#include "gui.hpp"

int GUI::wWidth;
int GUI::wHeight;
GLuint GUI::vertexBuffer;
GLuint GUI::uvBuffer;
GLuint GUI::textureID;
GLuint GUI::selectedTextureID;
GLuint GUI::mvpID;
GLuint GUI::shaderID;
ItemStack * GUI::blocks[8];
GUIImage * GUI::crosshair;
TextManager * GUI::textManager;
int GUI::selectedItemIndex = 0;

void
GUI::init(GLuint mvpid, int ww, int wh){
  static const float UVs[]{
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
  };
  glGenBuffers(1,&vertexBuffer);

  glGenBuffers(1,&uvBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(UVs), UVs, GL_STATIC_DRAW);
  textureID = ResourceManager::getTexture("textures/itembar.dds");
  selectedTextureID = ResourceManager::getTexture("textures/itembarselected.dds");
  mvpID = mvpid;
  wWidth = ww;
  wHeight = wh;
  shaderID =LoadShaders("shaders/gui.vertexshader", "shaders/gui.fragmentshader");
  textManager = new TextManager();
  textManager->init("textures/font.dds");
  crosshair = new GUIImage(
    mvpID,
    ResourceManager::getTexture("textures/crosshair.dds", false),
    shaderID,
    glm::vec4(620, 340, 660, 380)
  );
  refresh();
}

void
GUI::refresh(){
  float bottomMargin = 18.0f;
  float totalWidth = 1024.0f;
  float cellWidth = totalWidth/8.0f;
  float cellHeight = cellWidth;
  for(int i =0;i<8;i++){
    float xStart = 128.0f+i*128.0f;
    float xEnd = 256.0f+i*128.0f;
    float yStart = bottomMargin;
    float yEnd =  bottomMargin + cellHeight;
    xStart += cellWidth *0.2f;
    xEnd -= cellWidth *0.2f;
    yStart += cellHeight *0.2f;
    yEnd -= cellHeight *0.2f;
    Blocks::block_type blockType = Inventory::inventory[i].type;
    if(blockType != Blocks::NONE){
      GLuint texture = Blocks::Block::getTextureFor(blockType);
      if(blocks[i] == NULL){
        blocks[i] = new ItemStack(mvpID, texture, shaderID, glm::vec4(xStart,yStart,xEnd,yEnd), textManager);
      } else {
        blocks[i]->setTexture(texture);
      }
      blocks[i]->setCount(Inventory::inventory[i].count);
    } else {
      if(blocks[i] != NULL) delete blocks[i];
      blocks[i] = NULL;
    }
  }
}

void
GUI::dispose(){
  for(int i =0;i<8;i++){
    delete blocks[i];
  }
  delete crosshair;
  glDeleteBuffers(1, &vertexBuffer);
  glDeleteBuffers(1, &uvBuffer);
}

void
GUI::leftMouseButton(glm::vec2 mousePos, bool state){
  for(int i =0;i<8;i++){
    if(blocks[i] == NULL) continue;
    glm::vec4 box = blocks[i]->getPosition();
    if(mousePos.x > box.x && mousePos.x < box.z &&
      mousePos.y > box.y && mousePos.y < box.w){
        blocks[i]->setFollowMouse(state);
      }
  }
}

void
GUI::leaveGUI(){
  float bottomMargin = 18.0f;
  float totalWidth = 1024.0f;
  float cellWidth = totalWidth/8.0f;
  float cellHeight = cellWidth;
  for(int i =0;i<8;i++){
    float xStart = 128.0f+i*128.0f;
    float xEnd = 256.0f+i*128.0f;
    float yStart = bottomMargin;
    float yEnd =  bottomMargin + cellHeight;
    xStart += cellWidth *0.2f;
    xEnd -= cellWidth *0.2f;
    yStart += cellHeight *0.2f;
    yEnd -= cellHeight *0.2f;
    if(blocks[i] != NULL) blocks[i]->setPosition(glm::vec4(xStart,yStart,xEnd,yEnd));
  }
}

void
GUI::draw(glm::vec2 mousePos){
  glm::mat4 mvp = glm::mat4(1.0f);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);

  float bottomMargin = 18.0f;
  float totalWidth = 1024.0f;
  float cellWidth = totalWidth/8.0f;
  float cellHeight = cellWidth;
  for(int i =0;i<8;i++){
    float xStart = 128.0f+i*128.0f;
    float xEnd = 256.0f+i*128.0f;
    float yStart = bottomMargin;
    float yEnd =  bottomMargin + cellHeight;
    float vertices[]{
       xStart, yStart, 0.0f,
       xEnd, yEnd, 0.0f,
       xStart, yEnd, 0.0f,
       xStart, yStart, 0.0f,
       xEnd, yStart, 0.0f,
       xEnd, yEnd, 0.0f,
    };

    glActiveTexture(GL_TEXTURE0);
    if(i == selectedItemIndex){
      glBindTexture(GL_TEXTURE_2D, selectedTextureID);
      glUniform1i(selectedTextureID, 0);
    } else {
      glBindTexture(GL_TEXTURE_2D, textureID);
      glUniform1i(textureID, 0);
    }

    glUseProgram(shaderID);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(glm::vec3), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(
      0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
      3,                  // size
      GL_FLOAT,           // type
      GL_FALSE,           // normalized?
      0,                  // stride
      (void*)0            // array buffer offset
    );

    //color
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(
      1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
      2,                                // size
      GL_FLOAT,                         // type
      GL_FALSE,                         // normalized?
      0,                                // stride
      (void*)0                          // array buffer offset
    );

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
  }

  for(int i =0;i<8;i++){
    if(blocks[i] != NULL) blocks[i]->draw(mousePos);
  }
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  crosshair->draw();
}
