#ifndef B_GRASS_HPP
#define B_GRASS_HPP

#include "block.hpp"

namespace Blocks{
class Grass : public Block {
public:
  Grass();
  block_data getBlockData();
};
}
#endif
