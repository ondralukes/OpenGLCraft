#ifndef B_PICKAXE_HPP
#define B_PICKAXE_HPP

#include "block.hpp"

namespace Blocks{

class WoodenPickaxe : public Block {
public:
  WoodenPickaxe();
  block_data getBlockData();
};

class StonePickaxe : public Block {
public:
  StonePickaxe();
  block_data getBlockData();
};
}
#endif
