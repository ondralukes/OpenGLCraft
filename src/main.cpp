#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <thread>

#if defined(WIN32)
  #include <direct.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//#include "structs.hpp"
#include "blocks/blocks.hpp"
#include "chunk.hpp"
#include "loadShaders.hpp"
#include "worldGenerator.hpp"
#include "text.hpp"
#include "gui/gui.hpp"
#include "blockUtils.hpp"
#include "droppedBlock.hpp"
#include "resourceManager.hpp"
#include "ray.hpp"


const int wWidth = 1280;
const int wHeight = 720;
const float camHeight = 1.7f;
const float playerHeight = 1.75f;

int drawChunks(glm::mat4 projection, glm::mat4 view, intvec3 chunkPos);
void scrollCallback(GLFWwindow* window, double x, double y);
void recalcThWork(intvec3 * chunkPos);

int main(){
  srand(time(NULL));
  glewExperimental = true; // Needed for core profile
  if( !glfwInit() )
  {
    fprintf( stderr, "Failed to initialize GLFW\n" );
    return -1;
  }

  printf("Hit ESC to quit game\n");
  bool useFullscreen;

  char input;
  while(true){
    printf("Launch in fullscreen? [y/n]:");
    input = getchar();
    if(input == 'Y' || input == 'y'){
      useFullscreen = true;
      break;
    }
    if(input == 'N' || input == 'n'){
      useFullscreen = false;
      break;
    }
  }
  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

  GLFWwindow* window;
  window = glfwCreateWindow( wWidth, wHeight, "OpenGLCraft", useFullscreen?glfwGetPrimaryMonitor():NULL, NULL);
  if( window == NULL ){
    fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n" );
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glewExperimental=true;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    return -1;
  }
  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);
  GLuint shaderProgramID = LoadShaders( "shaders/object.vertexshader", "shaders/object.fragmentshader" );

  GLuint mvpID = glGetUniformLocation(shaderProgramID,"mvp");
  Chunk::mvpID = mvpID;

  #if defined(WIN32)
  _mkdir("saves");
  #else
  mkdir("saves",0777);
  #endif

  SaveManager * saveManager = new SaveManager("saves/default", mvpID);
  Chunk::saveManager = saveManager;
  saveManager->loadHeader();
  saveManager->loadInventory();
  Inventory::saveManager = saveManager;
  TextManager * text = new TextManager();
  text->init("textures/font.dds");
  GUI::init(mvpID, wWidth, wHeight);

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetScrollCallback(window, scrollCallback);

  glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  double xAngle = -10;
  double yAngle = -10;

  glm::vec3 camPos = saveManager->loadPlayerPos();
  glm::vec3 oldCamPos = camPos;

  WorldGenerator::generate(camPos,0.0f);


  double yVelocity = 0;
  double lastTime = glfwGetTime();
  double lastStatsTime = 0;
  double maxDrawTime = 0;
  double maxGenTime = 0;

  //Pointer for recalculation thread
  intvec3 * chunkPosPtr = new intvec3(0,0,0);
  std::thread * th = new std::thread(recalcThWork, chunkPosPtr);

  do{
    double time = glfwGetTime();
    float deltaTime =time-lastTime;
    lastTime = time;

    if(time - lastStatsTime > 2.0f){
      lastStatsTime = time;
      maxGenTime = 0;
      maxDrawTime = 0;
    }
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    glfwSetCursorPos(window, wWidth/2, wHeight/2);

    if(xAngle == -10){
      xAngle = saveManager->loadPlayerRot().x;
      yAngle = saveManager->loadPlayerRot().y;
    } else {
      xAngle -= (mouseX - wWidth/2)/100;
      yAngle -= (mouseY - wHeight/2)/100;
    }

    if(xAngle>M_PI*2)  xAngle -= M_PI*2;
    if(yAngle>M_PI/2)  yAngle = M_PI/2;
    if(xAngle<0)  xAngle += M_PI*2;
    if(yAngle<-M_PI/2)  yAngle = -M_PI/2;

    glm::vec3 camDirection(
      cos(yAngle) * sin(xAngle),
      sin(yAngle),
      cos(yAngle) * cos(xAngle)
    );

    glm::vec3 right = glm::vec3(
      sin(xAngle - M_PI/2.0f),
      0,
      cos(xAngle - M_PI/2.0f)
    );

    glm::vec3 forward = glm::vec3(
      sin(xAngle),
      0,
      cos(xAngle)
    );
    glm::vec3 up = glm::cross( right, camDirection );

    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS||glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
      camPos += forward*deltaTime*3.0f;
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS||glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
      camPos -= forward*deltaTime*3.0f;
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS||glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
      camPos -= right*deltaTime*3.0f;
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS||glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
      camPos += right*deltaTime*3.0f;
    }

    static bool spacePressed = false;
    static bool inAir = false;
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS&&!spacePressed){
      if(!inAir) yVelocity += 5.0f;
      spacePressed = true;
      inAir = true;
    } else if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE){
      spacePressed = false;
    }
    intvec3 blockPos(
      round(camPos.x),
      ceil(camPos.y-camHeight-0.5f),
      round(camPos.z)
    );

    static bool lMousePressed = false;
    static double lMousePressTime;
    static bool destroying;
    static intvec3 lMousePressRemoveBlock;
    int lMouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    intvec3 removePos;
    if(lMouseState == GLFW_PRESS){
      if(!lMousePressed){
        lMousePressTime = glfwGetTime();
        destroying = true;
      }

      if(getLookingAt(camPos,camDirection,&removePos)){
        if(!lMousePressed) lMousePressRemoveBlock = removePos;
        if(removePos == lMousePressRemoveBlock){
          removeBlock(removePos, time-lMousePressTime);
        }
      }
      lMousePressed = true;
    }
    if(lMouseState == GLFW_RELEASE || removePos != lMousePressRemoveBlock){
      removeBlock(lMousePressRemoveBlock, 0.0f);
      lMousePressed = false;
    }

    static bool rMousePressed = false;
    int rMouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if(rMouseState == GLFW_PRESS&&!rMousePressed){
      intvec3 placePos;
      if(getLookingAt(camPos,camDirection,NULL,&placePos)){
        if(placePos.x != blockPos.x || placePos.z != blockPos.z ||(placePos.y != blockPos.y+1 && placePos.y != blockPos.y+2)){
          Blocks::block_type blType = Inventory::getSelectedBlock();
          if(blType != Blocks::NONE){
            Blocks::block_data blData;
            blData.type = blType;
            Blocks::Block * block = Blocks::Block::decodeBlock(blData, placePos, mvpID);
            addBlock(placePos, block);
            Inventory::remove(GUI::selectedItemIndex);
          }
        }
      }
      rMousePressed = true;
    } else if(rMouseState == GLFW_RELEASE){
      rMousePressed = false;
    }


    yVelocity -= deltaTime*9.81f;
    inAir = true;
    if(isBlock(blockPos)){
      if(yVelocity < 0) yVelocity = 0;
      inAir = false;
    }

    camPos.y += yVelocity * deltaTime;

    intvec3 aboveBlockPos(
      round(camPos.x),
      ceil(camPos.y-camHeight+playerHeight-0.5),
      round(camPos.z)
    );
    if(isBlock(aboveBlockPos)){
      camPos.y = oldCamPos.y;
      yVelocity = 0;
    }
    for(int h = 1;h<4;h++){
      for(int d = -1;d<=1;d+=2){
        for(int r = 0;r<2;r++){
          float py;
          if(h==1) py = floor(camPos.y-camHeight+0.55f);
          if(h==2) py = floor(camPos.y-camHeight+1.35f);
          if(h==3) py = ceil(camPos.y-camHeight+1.35f);
          intvec3 p(
            round(camPos.x+ (r==0?d:0)),
            py,
            round(camPos.z+ (r==1?d:0))
          );
          float dx =  camPos.x - (blockPos.x);
          float dz =  camPos.z - (blockPos.z);

          if(
            (r==0&&d==1&&dx>0.35f)||
            (r==0&&d==-1&&dx<-0.35f)||
            (r==1&&d==1&&dz>0.35f)||
            (r==1&&d==-1&&dz<-0.35f))
            {
              if(isBlock(p)){
                if(r==0){
                  if(d == 1 && oldCamPos.x < camPos.x){
                    camPos.x = oldCamPos.x;
                  }
                  if(d == -1 && oldCamPos.x > camPos.x){
                    camPos.x = oldCamPos.x;
                  }
                } else {
                  if(d == 1 && oldCamPos.z < camPos.z){
                    camPos.z = oldCamPos.z;
                  }
                  if(d == -1 && oldCamPos.z > camPos.z){
                    camPos.z = oldCamPos.z;
                  }
                }
              }
            }
          }

        }
      }
      oldCamPos = camPos;
      glm::mat4 projection = glm::perspective(glm::radians(90.0f),wWidth/(float)wHeight, 0.01f, 100.0f);

      glm::mat4 view = glm::lookAt(
        camPos,
        camPos + camDirection,
        up
      );
      double genTime, drawTime;
      double start, end;
      start = glfwGetTime();
      WorldGenerator::generate(camPos,deltaTime);
      DroppedBlock::updateAll(deltaTime, camPos);
      end = glfwGetTime();
      genTime = end - start;
      if(genTime > maxGenTime) maxGenTime = genTime;
      // Clear the screen
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(shaderProgramID);

      intvec3 chunkPos(
        floor(blockPos.x/CHUNK_SIZE),
        floor(blockPos.y/CHUNK_SIZE) + 1,
        floor(blockPos.z/CHUNK_SIZE)
      );

      *chunkPosPtr = chunkPos;

      start = glfwGetTime();
      int chunksDrawed = drawChunks(projection,view,chunkPos);
      DroppedBlock::drawAll(projection,view);
      GUI::draw();
      end = glfwGetTime();
      drawTime = end - start;
      if(drawTime > maxDrawTime) maxDrawTime = drawTime;

      char textMsg[256];
      sprintf(textMsg,"fps :%.2f drw: %d chunks\n",1.0f/deltaTime, chunksDrawed);
      text->drawText(textMsg,glm::vec2(0,wHeight-28.0f),28.0f);
      sprintf(textMsg,"gen: %.2f (%.2f) drw: %.2f (%.2f)\n",genTime*1000, maxGenTime*1000, drawTime*1000, maxDrawTime*1000);
      text->drawText(textMsg,glm::vec2(0,wHeight-56.0f),28.0f);
      sprintf(textMsg,"pos: [%.2f;%.2f;%.2f]\n",camPos.x,camPos.y,camPos.z,yVelocity);
      text->drawText(textMsg,glm::vec2(0,wHeight-84.0f),28.0f);
      sprintf(textMsg,"blPos: [%d;%d;%d]\n",blockPos.x,blockPos.y,blockPos.z);
      text->drawText(textMsg,glm::vec2(0,wHeight-112.0f),28.0f);
      sprintf(textMsg,"yVel %.2f\n",yVelocity);
      text->drawText(textMsg,glm::vec2(0,wHeight-140.0f),28.0f);
      // Swap buffers
      glfwSwapBuffers(window);
      glfwPollEvents();
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0 );

    saveManager->savePlayerPos(camPos);
    saveManager->savePlayerRot(glm::vec2(xAngle, yAngle));
    saveManager->saveInventory();

    saveManager->compress();
    saveManager->cleanUp();

    delete saveManager;
    GUI::dispose();

    glDeleteProgram(shaderProgramID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
  }

int drawChunks(glm::mat4 projection, glm::mat4 view, intvec3 chunkPos){
  int chunksDrawed = 0;
  for(int x =-8;x<8;x++){
    for(int y =-4;y<4;y++){
      for(int z =-8;z<8;z++){
        intvec3 chP(
          chunkPos.x + x,
          chunkPos.y + y,
          chunkPos.z + z
        );
        Chunk * ch = Chunk::getChunk(chP);
        if(ch != NULL){
          if(ch->doDraw != 0){
            ch->draw(projection,view);
            chunksDrawed++;
          }
        }
      }
    }
  }
  return chunksDrawed;
}

void scrollCallback(GLFWwindow* window, double x, double y){
  GUI::selectedItemIndex += y;
  GUI::selectedItemIndex = (GUI::selectedItemIndex + 8)%8;
}

void recalcThWork(intvec3 * chunkPos){
  while(true){
    for(int x =-8;x<8;x++){
      for(int y =-4;y<4;y++){
        for(int z =-8;z<8;z++){
          intvec3 chP(
            chunkPos->x + x,
            chunkPos->y + y,
            chunkPos->z + z
          );
          Chunk * ch = Chunk::getChunk(chP);
          if(ch != NULL){
            if(ch->shouldRecalculate){
              ch->recalculate();
            }
          }
        }
      }
    }
  }
}
