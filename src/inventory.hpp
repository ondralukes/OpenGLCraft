#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include "blocks/block.hpp"
#include "gui/gui.hpp"
#include "saveManager.hpp"

class SaveManager;

typedef struct inventory_item {
  Blocks::block_type type = Blocks::NONE;
  int count = 0;
};
class Inventory {
public:
  static void add(Blocks::block_type type, int count = 1);
  static void remove(int index, int count = 1);
  static Blocks::block_type getSelectedBlock();
  static inventory_item inventory[8];
  static SaveManager * saveManager;
};

#endif
