#ifndef B_FURNACE_HPP
#define B_FURNACE_HPP

#include "block.hpp"
#include "inventory.hpp"
#include "gui/gui.hpp"

struct inventory_item;

namespace Blocks{
class Furnace : public Block {
public:
  Furnace();
  void save();
  void load();
  bool rightClick();
  void update();
  void destroy();
  float fuel = 0.0f;
  double startTime;
  float blockFuelBurnt = 0.0f;
  bool burning = false;
  inventory_item * content;
  static constexpr float speed = 0.1f;
};
}
#endif
