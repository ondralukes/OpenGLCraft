#ifndef ITEM_FIELD_HPP
#define ITEM_FIELD_HPP

#include <glm/glm.hpp>

#include "itemStack.hpp"
#include "gui.hpp"
#include "guiImage.hpp"
#include "resourceManager.hpp"

class ItemField {
public:
  ItemField(glm::vec2 p);
  ~ItemField();
  void put(int stackIndex, bool right, bool force = false);
  void get(int stackIndex, bool right);
  void empty();
  void draw();
  void setTexture(GLuint texID);
  void removeContentPointer();
  glm::vec2 getPosition();
  ItemStack * getContent();
  bool drop = true;
  bool getOnly = false;
  int minGet = -1;

  void (*onGet)(int) = NULL;
  void (*onPut)(void)= NULL;
private:
  glm::vec2 pos;
  int contentIndex = -1;
  GUIImage * img;
};

#endif
