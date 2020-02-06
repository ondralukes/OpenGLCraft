#ifndef B_AXE_HPP
#define B_AXE_HPP

#include "block.hpp"
#include "saveManager.hpp"

namespace Blocks{

class WoodenAxe : public Block {
public:
  WoodenAxe();
  void save();
  void load();
  bool usedAsTool();
  float getHP();
private:
  int hp;
};

class StoneAxe : public Block {
public:
  StoneAxe();
  void save();
  void load();
  bool usedAsTool();
  float getHP();
private:
  int hp;
};
}
#endif
