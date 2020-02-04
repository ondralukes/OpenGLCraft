#ifndef ITEM_FIELD_HPP
#define ITEM_FIELD_HPP

#include <glm/glm.hpp>

#include "itemStack.hpp"
#include "gui.hpp"

class ItemField {
public:
  ItemField(glm::vec2 p);
  void put(int stackIndex);
  void empty();
  void removeContentPointer();
  glm::vec2 getPosition();
  ItemStack * getContent();
private:
  glm::vec2 pos;
  int contentIndex = -1;
};

#endif
