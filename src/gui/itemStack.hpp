#ifndef ITEM_STACK_HPP
#define ITEM_STACK_HPP

#include <glm/glm.hpp>
#include "guiImage.hpp"
#include "text.hpp"

class ItemStack : public GUIImage{
public:
  ItemStack(GLuint mvpid, GLuint texID, GLuint shaderID, glm::vec4 pos, TextManager * textManager);
  void setCount(int c);
  void setFollowMouse(bool b);
  bool getFollowMouse();
  int getCount();
  void draw(glm::vec2 mousePos);
private:
  int count = 1;
  char countText[32];
  TextManager * textManager;
  bool followMouse = false;
};

#endif
