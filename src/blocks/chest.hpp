#ifndef B_CHEST_HPP
#define B_CHEST_HPP

#include "inventory.hpp"
#include "block.hpp"
#include "gui/gui.hpp"

struct inventory_item;

namespace Blocks{
class Chest : public Block {
public:
  Chest();
  void destroy();
  void save();
  void load();
  bool rightClick();
  void setContent(int index, inventory_item data);
  inventory_item getContent(int index);
private:
  inventory_item * content;
};
}
#endif
