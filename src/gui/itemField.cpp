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
  } else if(GUI::blocks[contentIndex]->getBlock()->getType() == GUI::blocks[stackIndex]->getBlock()->getType()){
    //Add to stack
    int maxStack = GUI::blocks[contentIndex]->getBlock()->maxStack;
    int countToAdd = maxStack - GUI::blocks[contentIndex]->getCount();
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
}
