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
GUIImage * GUI::craftingGrid;
TextManager * GUI::textManager;
int GUI::selectedItemIndex = 0;
bool GUI::inGUI = false;
int GUI::currentDraggingIndex = -1;
ItemField * GUI::itemFields;
int GUI::craftingOutputIndex;

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
  craftingGrid = new GUIImage(
    mvpID,
    ResourceManager::getTexture("textures/craftinggrid.dds", false),
    shaderID,
    glm::vec4(440, 160, 840, 560)
  );

  itemFields = (ItemField *) malloc(17 * sizeof(ItemField));
  for(int x = 0;x < 3;x++){
    for(int y = 0;y < 3;y++){
      itemFields[x+y*3] = ItemField(glm::vec2(507 + x*133, 227 + (2-y)*133));
    }
  }

  for(int i =0;i< 8;i++){
    itemFields[9 + i] = ItemField(glm::vec2(192 + i*128, 82));
  }
  refresh();
}

void
GUI::refresh(){
  for(int i = 0;i<17;i++){
    itemFields[i].removeContentPointer();
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
      itemFields[9+i].put(i);
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
GUI::mouseButton(glm::vec2 mousePos, bool right, bool state){
  if(!state) return;
  for(int i =0;i<blocks.size();i++){
    if(blocks[i] == NULL) continue;
    glm::vec4 box = blocks[i]->getPosition();
    if(mousePos.x > box.x && mousePos.x < box.z &&
      mousePos.y > box.y && mousePos.y < box.w){
        if(currentDraggingIndex != -1){
          //Stick to nearest item field
          float nearestD = 1000.0f;
          ItemField * bestField;
          for(int j = 0;j<17;j++){
            float d = glm::length(mousePos - itemFields[j].getPosition());
            if(d < nearestD){
              nearestD = d;
              bestField = &(itemFields[j]);
            }
          }
          if(nearestD < 128.0f){
            glm::vec4 pos = blocks[currentDraggingIndex]->getPosition();
            glm::vec2 size = glm::vec2(pos.z - pos.x, pos.w - pos.y);
            glm::vec2 fpos = bestField->getPosition();
            blocks[currentDraggingIndex]->setPosition(
              glm::vec4(
                fpos.x - size.x/2.0f,
                fpos.y - size.y/2.0f,
                fpos.x + size.x/2.0f,
                fpos.y + size.y/2.0f
              )
            );
            bestField->put(currentDraggingIndex);
            if(blocks[currentDraggingIndex] == NULL){
               currentDraggingIndex = -1;
             }
          }
        } else {
          ItemStack * newStack = new ItemStack(mvpID, blocks[i]->getTexture(), shaderID, blocks[i]->getPosition(), textManager, blocks[i]->getBlock());
          int count = blocks[i]->getCount();
          if(!right || i == craftingOutputIndex){
            if(count > blocks[i]->getBlock()->maxStack) count = blocks[i]->getBlock()->maxStack;
            if(count > blocks[i]->getCount()) count = blocks[i]->getCount();
            blocks[i]->setCount(blocks[i]->getCount() - count);
            if(blocks[i]->getCount() == 0){
              delete blocks[i];
              blocks[i] = NULL;
            }
          } else {
            blocks[i]->setCount(count/2);
            if(count/2 == 0){
              delete blocks[i];
              blocks[i] = NULL;
            }
            count -= count/2;
          }
          newStack->setCount(count);

          blocks.push_back(newStack);
          currentDraggingIndex = blocks.size() - 1;
          blocks[currentDraggingIndex]->setFollowMouse(true);

          //Remove used items from crating grid
          if(i == craftingOutputIndex){
            for(int x = 0;x < 3;x++){
              for(int y = 0;y < 3;y++){
                if(itemFields[x+y*3].getContent() != NULL){
                  int c = itemFields[x+y*3].getContent()->getCount();
                  if(c < count) count = c;
                }
              }
            }
            for(int x = 0;x < 3;x++){
              for(int y = 0;y < 3;y++){
                if(itemFields[x+y*3].getContent() == NULL) continue;
                int c = itemFields[x+y*3].getContent()->getCount();
                itemFields[x+y*3].getContent()->setCount(c-count);
                if(c - count <= 0){
                  itemFields[x+y*3].empty();
                }
              }
            }
          }
        }

        break;
      }
  }
  updateCraftingResult();
}

void
GUI::leaveGUI(glm::vec3 playerPos){
  inGUI = false;
  //Update inventory
  for(int i =0;i<8;i++){
    ItemStack * content = itemFields[9 + i].getContent();
    if(content == NULL)
    {
      Inventory::inventory[i].block = NULL;
    } else {
      Inventory::inventory[i].block = content->getBlock();
      Inventory::inventory[i].count = content->getCount();
    }
  }
  //Remove blocks at crafting grid and drop them
  for(int i = 0; i < 9;i++){
    ItemStack * content = itemFields[i].getContent();
    if(content != NULL){
      for(int j =0;j<content->getCount();j++){
        DroppedBlock * drop = new DroppedBlock(mvpID, content->getBlock(), playerPos);
        glm::vec3 vel(
          ((rand()%40) - 20)* 0.01f,
          1.5f,
          ((rand()%40) - 20)* 0.01f
        );
        drop->setVelocity(vel);
      }
    }
    itemFields[i].empty();
  }

  //Drop dragged item
  if(currentDraggingIndex != -1){
    ItemStack * content = blocks[currentDraggingIndex];
    for(int j =0;j<content->getCount();j++){
      DroppedBlock * drop = new DroppedBlock(mvpID, content->getBlock(), playerPos);
      glm::vec3 vel(
        ((rand()%40) - 20)* 0.01f,
        1.5f,
        ((rand()%40) - 20)* 0.01f
      );
      drop->setVelocity(vel);
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
GUI::updateCraftingResult(){
  Blocks::block_data input[3][3];
  int count = 999999999;
  bool empty = true;
  for(int x = 0;x < 3;x++){
    for(int y = 0;y < 3;y++){
      if(itemFields[x+y*3].getContent() == NULL){
        input[x][y].type = Blocks::NONE;
        input[x][y].dataPos = 0;
      } else {
        input[x][y] = itemFields[x+y*3].getContent()->getBlock()->getBlockData();
        int c = itemFields[x+y*3].getContent()->getCount();
        if(c < count) count = c;
        empty = false;
      }
    }
  }
  if(empty) return;
  //Move to left
  while(true){
    bool move = true;
    for(int y = 0;y<3;y++){
      if(input[0][y].type != Blocks::NONE) move = false;
    }
    if(!move) break;
    for(int x = 0;x<2;x++){
      for(int y = 0;y < 3;y++){
        input[x][y] = input[x+1][y];
        input[x+1][y].type = Blocks::NONE;
        input[x+1][y].dataPos = 0;
      }
    }
  }

  //Move to top corner
  while(true){
    bool move = true;
    for(int x = 0;x<3;x++){
      if(input[x][0].type != Blocks::NONE) move = false;
    }
    if(!move) break;
    for(int y = 0;y<2;y++){
      for(int x = 0;x < 3;x++){
        input[x][y] = input[x][y+1];
        input[x][y+1].type = Blocks::NONE;
        input[x][y+1].dataPos = 0;
      }
    }
  }

  bool foundRecipe = false;
  for(int i =0;i<Recipes::recipesCount;i++){
    if(Recipes::recipes[i].doesMatch(input)){
      Blocks::Block * block = Blocks::Block::decodeBlock(Recipes::recipes[i].getOutput(), intvec3(0,0,0), 0);
      block->doDrop = false;
      blocks[craftingOutputIndex] = new ItemStack(
        mvpID,
        Blocks::Block::getTextureFor(block->getType()),
        shaderID,
        glm::vec4(1000.6f, 321.6f, 1077.4f, 398.4f),
        textManager,
        block);
      blocks[craftingOutputIndex]->setCount(count *  Recipes::recipes[i].getCount());
      foundRecipe = true;
      break;
    }
  }
  if(!foundRecipe){
    if(blocks[craftingOutputIndex] != NULL) delete blocks[craftingOutputIndex];
    blocks[craftingOutputIndex] = NULL;
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

  if(inGUI){
    craftingGrid->draw();
  } else {
    crosshair->draw();
  }

  for(int i =0;i<blocks.size();i++){
    if(blocks[i] != NULL) blocks[i]->draw(mousePos);
  }
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

}
