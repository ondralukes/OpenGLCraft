#ifndef B_SHOVEL_HPP
#define B_SHOVEL_HPP

#include "block.hpp"
#include "saveManager.hpp"

namespace Blocks{

class WoodenShovel : public Block {
public:
  WoodenShovel();
  void save();
  void load();
  bool usedAsTool();
  float getHP();
private:
  int hp;
};

class StoneShovel : public Block {
public:
  StoneShovel();
  void save();
  void load();
  bool usedAsTool();
  float getHP();
private:
  int hp;
};
}
#endif
