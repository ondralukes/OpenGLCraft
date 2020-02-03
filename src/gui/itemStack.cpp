#include "itemStack.hpp"

ItemStack::ItemStack(GLuint mvpid, GLuint texID, GLuint shaderID, glm::vec4 pos, TextManager * textManager)
  : GUIImage(mvpid, texID, shaderID, pos), textManager(textManager){
    setCount(1);
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
  textManager->drawText(countText, glm::vec2(pos.x, pos.y), 28.0f);
}
