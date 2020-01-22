#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

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
#include "blockUtils.hpp"
#include "ray.hpp"


const int wWidth = 1280;
const int wHeight = 720;
const float camHeight = 1.7f;
const float playerHeight = 1.75f;

void drawChunks(glm::mat4 projection, glm::mat4 view, intvec3 chunkPos);

int main(){
  glewExperimental = true; // Needed for core profile
  if( !glfwInit() )
  {
    fprintf( stderr, "Failed to initialize GLFW\n" );
    return -1;
  }
  glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

  GLFWwindow* window;
  window = glfwCreateWindow( wWidth, wHeight, "OpenGLCraft", glfwGetPrimaryMonitor(), NULL);
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

  mkdir("saves", 0777);
  SaveManager * saveManager = new SaveManager("saves/default");
  Chunk::saveManager = saveManager;
  saveManager->loadHeader();
  TextManager * text = new TextManager();
  text->init("textures/font.dds");

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);

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

  do{
    double time = glfwGetTime();
    float deltaTime =time-lastTime;
    lastTime = time;
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
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS&&!spacePressed){
      yVelocity += 5.0f;
      spacePressed = true;
    } else if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE){
      spacePressed = false;
    }
    intvec3 blockPos(
      round(camPos.x),
      round(camPos.y-camHeight),
      round(camPos.z)
    );

    static bool lMousePressed = false;
    int lMouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if(lMouseState == GLFW_PRESS&&!lMousePressed){
      intvec3 removePos;
      if(getLookingAt(camPos,camDirection,&removePos)){
        removeBlock(removePos);
      }
      lMousePressed = true;
    } else if(lMouseState == GLFW_RELEASE){
      lMousePressed = false;
    }

    static bool rMousePressed = false;
    int rMouseState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if(rMouseState == GLFW_PRESS&&!rMousePressed){
      intvec3 placePos;
      if(getLookingAt(camPos,camDirection,NULL,&placePos)){
        if(placePos.x != blockPos.x || placePos.z != blockPos.z ||(placePos.y != blockPos.y+1 && placePos.y != blockPos.y+2)){
          addBlock(placePos, new Blocks::Stone());
        }
      }
      rMousePressed = true;
    } else if(rMouseState == GLFW_RELEASE){
      rMousePressed = false;
    }


    yVelocity -= deltaTime*9.81f;
    if(isBlock(blockPos)){
      if(yVelocity < 0) yVelocity = 0;
    }

    camPos.y += yVelocity * deltaTime;

    intvec3 aboveBlockPos(
      round(camPos.x),
      round(camPos.y-camHeight+playerHeight),
      round(camPos.z)
    );
    if(isBlock(aboveBlockPos)){
      camPos.y = oldCamPos.y;
      yVelocity = 0;
    }
    for(int h = 1;h<3;h++){
      for(int d = -1;d<=1;d+=2){
        for(int r = 0;r<2;r++){
          intvec3 p(
            blockPos.x + (r==0?d:0),
            blockPos.y + h,
            blockPos.z + (r==1?d:0)
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
      end = glfwGetTime();
      genTime = end - start;
      // Clear the screen
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(shaderProgramID);

      intvec3 chunkPos(
        floor(blockPos.x/CHUNK_SIZE),
        floor(blockPos.y/CHUNK_SIZE) + 1,
        floor(blockPos.z/CHUNK_SIZE)
      );

      start = glfwGetTime();
      drawChunks(projection,view,chunkPos);
      end = glfwGetTime();
      drawTime = end - start;

      char textMsg[256];
      sprintf(textMsg,"fps :%.2f\n",1.0f/deltaTime);
      text->drawText(textMsg,glm::vec2(0,0),28.0f);
      sprintf(textMsg,"gen: %.2f drw: %.2f\n",genTime*1000,drawTime*1000);
      text->drawText(textMsg,glm::vec2(0,28.0f),28.0f);
      sprintf(textMsg,"pos: [%.2f;%.2f;%.2f]\n",camPos.x,camPos.y,camPos.z,yVelocity);
      text->drawText(textMsg,glm::vec2(0,56.0f),28.0f);
      sprintf(textMsg,"blPos: [%d;%d;%d]\n",blockPos.x,blockPos.y,blockPos.z);
      text->drawText(textMsg,glm::vec2(0,84.0f),28.0f);
      sprintf(textMsg,"yVel %.2f\n",yVelocity);
      text->drawText(textMsg,glm::vec2(0,112.0f),28.0f);
      // Swap buffers
      glfwSwapBuffers(window);
      glfwPollEvents();
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0 );

    saveManager->savePlayerPos(camPos);
    saveManager->savePlayerRot(glm::vec2(xAngle, yAngle));

    saveManager->compress();
    saveManager->cleanUp();

    delete saveManager;

    glDeleteProgram(shaderProgramID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
  }

  void drawChunks(glm::mat4 projection, glm::mat4 view, intvec3 chunkPos){
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
            }
          }
        }
      }
    }
  }
