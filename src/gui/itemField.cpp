#include "itemField.hpp"

ItemField::ItemField(glm::vec2 p){
  pos = p;
  img = new GUIImage(
    GUI::mvpID,
    ResourceManager::getTexture("textures/itembar.dds", false),
    GUI::shaderID,
    glm::vec4(p-glm::vec2(64,64), p+glm::vec2(64,64))
  );
}

ItemField::~ItemField(){
  delete img;
}

glm::vec2
ItemField::getPosition(){
  return pos;
}

void
ItemField::removeContentPointer(){
  contentIndex = -1;
}

ItemStack *
ItemField::getContent(){
  if(contentIndex == -1) return NULL;
  return GUI::blocks[contentIndex];
}


void
ItemField::empty(){
  if(contentIndex == -1) return;
  if(GUI::blocks[contentIndex] != NULL){
    delete GUI::blocks[contentIndex];
  }
  GUI::blocks[contentIndex] = NULL;
  contentIndex = -1;
}

void
ItemField::put(int stackIndex, bool right, bool force){
  if(getOnly && !force) return;
  if(contentIndex == -1){
    GUI::blocks.push_back(NULL);
    contentIndex = GUI::blocks.size()-1;
  }

  if(GUI::blocks[contentIndex] == NULL){
    //Put to empty fiels
    if(right){
      GUI::blocks[contentIndex] = GUI::blocks[stackIndex]->getClone();
      GUI::blocks[stackIndex]->setCount(GUI::blocks[stackIndex]->getCount() - 1);
      GUI::blocks[contentIndex]->setCount(1);
      GUI::blocks[contentIndex]->setFollowMouse(false);
    } else {
      GUI::blocks[contentIndex] = GUI::blocks[stackIndex];
      GUI::blocks[contentIndex]->setFollowMouse(false);
      GUI::blocks[stackIndex] = NULL;
    }
  } else if(GUI::blocks[contentIndex]->getBlock()->getType() == GUI::blocks[stackIndex]->getBlock()->getType()){
    //Add to stack
    int maxStack = GUI::blocks[contentIndex]->getBlock()->maxStack;
    int countToAdd = maxStack - GUI::blocks[contentIndex]->getCount();
    if(right) if(countToAdd>0) countToAdd = 1;
    if(countToAdd > GUI::blocks[stackIndex]->getCount()) countToAdd = GUI::blocks[stackIndex]->getCount();
    int count = GUI::blocks[contentIndex]->getCount() + countToAdd;
    GUI::blocks[contentIndex]->setCount(count);
    GUI::blocks[contentIndex]->setFollowMouse(false);
    if(countToAdd == GUI::blocks[stackIndex]->getCount()){
      delete GUI::blocks[stackIndex];
      GUI::blocks[stackIndex] = NULL;
    } else {
      GUI::blocks[stackIndex]->setCount(GUI::blocks[stackIndex]->getCount() - countToAdd);
      GUI::blocks[contentIndex]->setFollowMouse(false);
      GUI::blocks[stackIndex]->setFollowMouse(true);
    }
  } else {
    //Swap
    ItemStack * tmp = GUI::blocks[contentIndex];
    GUI::blocks[contentIndex] = GUI::blocks[stackIndex];
    GUI::blocks[stackIndex] = tmp;
    GUI::blocks[contentIndex]->setFollowMouse(false);
    GUI::blocks[stackIndex]->setFollowMouse(true);
  }
  if(onPut != NULL) onPut();
}

void
ItemField::get(int stackIndex, bool right){
  if(contentIndex == -1) return;
  if(GUI::blocks[contentIndex] == NULL) return;
  if(right){
    int count = GUI::blocks[contentIndex]->getCount();
    GUI::blocks[stackIndex] = GUI::blocks[contentIndex]->getClone();
    if(minGet == -1){
      GUI::blocks[contentIndex]->setCount(count/2);
      if(count/2 == 0){
        delete GUI::blocks[contentIndex];
        GUI::blocks[contentIndex] = NULL;
      }
      count -= count/2;
    } else {
      GUI::blocks[contentIndex]->setCount(count - minGet);
      if(count - minGet == 0){
        delete GUI::blocks[contentIndex];
        GUI::blocks[contentIndex] = NULL;
      }
      count -= count - minGet;
    }
    GUI::blocks[stackIndex]->setCount(count);
    GUI::blocks[stackIndex]->setFollowMouse(true);
    if(onGet != NULL) onGet(count);
  } else {
    int count = GUI::blocks[contentIndex]->getCount();
    GUI::blocks[stackIndex] = GUI::blocks[contentIndex];
    GUI::blocks[stackIndex]->setFollowMouse(true);
    GUI::blocks[contentIndex] = NULL;
    if(onGet != NULL) onGet(count);
  }
}

void
ItemField::setTexture(GLuint texID){
  img->setTexture(texID);
}
void
ItemField::draw(){
  img->draw();
}
