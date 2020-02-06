#ifndef B_PICKAXE_HPP
#define B_PICKAXE_HPP

#include "block.hpp"
#include "saveManager.hpp"

namespace Blocks{

class WoodenPickaxe : public Block {
public:
  WoodenPickaxe();
  void save();
  void load();
  bool usedAsTool();
  float getHP();
private:
  int hp;
};

class StonePickaxe : public Block {
public:
  StonePickaxe();
  void save();
  void load();
  bool usedAsTool();
  float getHP();
private:
  int hp;
};
}
#endif
