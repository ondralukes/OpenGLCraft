#include "itemStack.hpp"

ItemStack::ItemStack(GLuint mvpid, GLuint texID, GLuint shaderID, glm::vec4 pos, TextManager * textManager, Blocks::Block * bl)
  : GUIImage(mvpid, texID, shaderID, pos), textManager(textManager){
    setCount(1);
    block = bl;
  }

Blocks::Block *
ItemStack::getBlock(){
  return block;
}

void
ItemStack::setFollowMouse(bool b){
  followMouse = b;
}

bool
ItemStack::getFollowMouse(){
  return followMouse;
}

void
ItemStack::setCount(int c){
  count = c;
  sprintf(countText, "%d", c);
}

int
ItemStack::getCount(){
  return count;
}

void
ItemStack::draw(glm::vec2 mousePos){
  if(followMouse){
    float width = pos.z - pos.x;
    float height = pos.w - pos.y;
    glm::vec4 p = glm::vec4(
      mousePos.x-width/2.0f,
      mousePos.y-height/2.0f,
      mousePos.x+width/2.0f,
      mousePos.y+height/2.0f
    );
    setPosition(p);
  }
  GUIImage::draw();
  if(count != 1){
    textManager->drawText(countText, glm::vec2(pos.x, pos.y), 28.0f);
  }
  float hp = block->getHP();
  if(hp != 1.0f){
    char hpText[32];
    sprintf(hpText, "%.0f%%", hp*100);
    textManager->drawText(hpText, glm::vec2(pos.x, pos.y+48.0f), 28.0f);
  }
}
