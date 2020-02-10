#include "gui.hpp"

int GUI::wWidth;
int GUI::wHeight;
GLuint GUI::vertexBuffer;
GLuint GUI::uvBuffer;
GLuint GUI::textureID;
GLuint GUI::selectedTextureID;
GLuint GUI::mvpID;
GLuint GUI::shaderID;
std::vector<ItemStack *> GUI::blocks;
GUIImage * GUI::crosshair;
TextManager * GUI::textManager;
int GUI::selectedItemIndex = 0;
bool GUI::inGUI = false;
int GUI::currentDraggingIndex = -1;
int GUI::itemFieldsCount = 0;
ItemField * GUI::itemFields;
int GUI::craftingOutputIndex;
GUIBase * GUI::currentGUI = NULL;

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

  reload();
}

void
GUI::reload(){
  for(int i = 0;i<itemFieldsCount;i++){
    itemFields[i].removeContentPointer();
  }

  itemFieldsCount = 8;
  if(currentGUI != NULL) itemFieldsCount += currentGUI->itemFieldsCount;
  itemFields = (ItemField *) malloc(itemFieldsCount * sizeof(ItemField));
  for(int i =0;i < 8;i++){
    itemFields[i] = ItemField(glm::vec2(192 + i*128, 82));
    itemFields[i].drop = false;
  }
  if(currentGUI != NULL){
    currentGUI->generateFields(&itemFields[8]);
  }

  blocks.clear();
  blocks.resize(8);
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
    Blocks::Block * block = Inventory::inventory[i].block;
    if(block != NULL){
      GLuint texture = Blocks::Block::getTextureFor(block->getType());
      if(blocks[i] == NULL){
        blocks[i] = new ItemStack(mvpID, texture, shaderID, glm::vec4(xStart,yStart,xEnd,yEnd), textManager, block);
      } else {
        blocks[i]->setTexture(texture);
      }
      blocks[i]->setCount(Inventory::inventory[i].count);
      itemFields[i].put(i, false);
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
GUI::mouseButton(glm::vec2 mousePos, bool right, bool state, glm::vec3 pos, glm::vec3 dir){
  if(!state) return;
  bool drop = true;
  for(int i =0;i<itemFieldsCount;i++){
    glm::vec2 p = itemFields[i].getPosition();
    if(mousePos.x > p.x - 64 && mousePos.x < p.x + 64 &&
      mousePos.y > p.y - 64 && mousePos.y < p.y + 64){
        drop = false;
        if(currentDraggingIndex != -1){
          glm::vec4 pos = blocks[currentDraggingIndex]->getPosition();
            glm::vec2 size = glm::vec2(pos.z - pos.x, pos.w - pos.y);
            blocks[currentDraggingIndex]->setPosition(
              glm::vec4(
                p.x - size.x/2.0f,
                p.y - size.y/2.0f,
                p.x + size.x/2.0f,
                p.y + size.y/2.0f
              )
            );
          itemFields[i].put(currentDraggingIndex, right);
          if(blocks[currentDraggingIndex] == NULL){
            currentDraggingIndex = -1;
          }
        } else {
          blocks.push_back(NULL);
          currentDraggingIndex = blocks.size()-1;
          itemFields[i].get(currentDraggingIndex, right);
        }
        if(blocks[currentDraggingIndex] == NULL || currentDraggingIndex == -1){
          currentDraggingIndex = -1;
        } else if(blocks[currentDraggingIndex]->getCount() <= 0){
          delete blocks[currentDraggingIndex];
          blocks[currentDraggingIndex] = NULL;
          currentDraggingIndex = -1;
        }
      }
    }
    if(drop) {
      dropSelected(pos,dir);
    }
  }

void
GUI::dropSelected(glm::vec3 pos, glm::vec3 dir){
  if(currentDraggingIndex == -1) return;
  Block * droppedBlock = Blocks::Block::decodeBlock(blocks[currentDraggingIndex]->getBlock()->getBlockData(), intvec3(0,0,0), mvpID);
  for(int i =0;i<blocks[currentDraggingIndex]->getCount();i++){
    DroppedBlock * drop = new DroppedBlock(mvpID, droppedBlock, pos);
    float speed = (rand()%2000)/1000.0f + 4.0f;
    drop->setVelocity(dir*speed);
    drop->canPick = false;
  }
  blocks[GUI::currentDraggingIndex] = NULL;
  currentDraggingIndex = -1;

}

void
GUI::leaveGUI(glm::vec3 pos, glm::vec3 dir){
  inGUI = false;
  if(blocks[craftingOutputIndex] != NULL) delete blocks[craftingOutputIndex];
  blocks[craftingOutputIndex] = NULL;
  //Update inventory
  for(int i =0;i<8;i++){
    ItemStack * content = itemFields[i].getContent();
    if(content == NULL)
    {
      Inventory::inventory[i].block = NULL;
    } else {
      Inventory::inventory[i].block = content->getBlock();
      Inventory::inventory[i].count = content->getCount();
    }
  }
  //Remove blocks at crafting grid and drop them
  for(int i = 0; i < itemFieldsCount;i++){
    if(!itemFields[i].drop) continue;
    ItemStack * content = itemFields[i].getContent();
    if(content != NULL){
      for(int j =0;j<content->getCount();j++){
        DroppedBlock * drop = new DroppedBlock(mvpID, content->getBlock(), pos);
        float speed = (rand()%2000)/1000.0f + 4.0f;
        drop->setVelocity(dir*speed);
        drop->canPick = false;
      }
    }
    itemFields[i].empty();
  }

  //Drop dragged item
  if(currentDraggingIndex != -1){
    ItemStack * content = blocks[currentDraggingIndex];
    for(int j =0;j<content->getCount();j++){
      DroppedBlock * drop = new DroppedBlock(mvpID, content->getBlock(), pos);
      float speed = (rand()%2000)/1000.0f + 4.0f;
      drop->setVelocity(dir*speed);
      drop->canPick = false;
    }
    delete content;
    blocks[currentDraggingIndex] = NULL;
  }
  currentDraggingIndex = -1;
}

void
GUI::enterGUI(){
  inGUI = true;

  //Create item stack for crafting output
  blocks.push_back(NULL);
  craftingOutputIndex = blocks.size() - 1;
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

  if(!inGUI){
    crosshair->draw();
  }

  for(int i =0;i<blocks.size();i++){
    if(blocks[i] != NULL) blocks[i]->draw(mousePos);
  }
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

}
