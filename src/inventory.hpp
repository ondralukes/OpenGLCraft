#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include "blocks/block.hpp"
#include "gui/gui.hpp"
#include "saveManager.hpp"

class SaveManager;

typedef struct inventory_item {
  Blocks::Block * block = NULL;
  int count = 0;
};
class Inventory {
public:
  static void add(Blocks::Block * block, int count = 1);
  static void remove(int index, int count = 1);
  static bool isPlaceFor(Blocks::block_data block);
  static Blocks::Block * getSelectedBlock();
  static int getSelectedCount();
  static void destroySelectedBlock(bool del = true);
  static inventory_item inventory[8];
};

#endif
