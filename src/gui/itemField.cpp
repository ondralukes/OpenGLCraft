#include "itemField.hpp"

ItemField::ItemField(glm::vec2 p){
  pos = p;
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
ItemField::put(int stackIndex){
  if(contentIndex == -1){
    GUI::blocks.push_back(NULL);
    contentIndex = GUI::blocks.size()-1;
  }

  if(GUI::blocks[contentIndex] == NULL){
    //Put to empty fiels
    GUI::blocks[contentIndex]  = GUI::blocks[stackIndex];
    GUI::blocks[contentIndex]->setFollowMouse(false);
    GUI::blocks[stackIndex] = NULL;
  } else if(GUI::blocks[contentIndex]->getBlockType() == GUI::blocks[stackIndex]->getBlockType()){
    //Add to stack
    int count = GUI::blocks[contentIndex]->getCount() + GUI::blocks[stackIndex]->getCount();
    GUI::blocks[contentIndex]->setCount(count);
    GUI::blocks[contentIndex]->setFollowMouse(false);
    delete GUI::blocks[stackIndex];
    GUI::blocks[stackIndex] = NULL;
  } else {
    //Swap
    ItemStack * tmp = GUI::blocks[contentIndex];
    GUI::blocks[contentIndex] = GUI::blocks[stackIndex];
    GUI::blocks[stackIndex] = tmp;
    GUI::blocks[contentIndex]->setFollowMouse(false);
    GUI::blocks[stackIndex]->setFollowMouse(true);
  }
}
