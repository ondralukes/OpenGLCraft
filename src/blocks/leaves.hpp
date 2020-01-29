#ifndef B_LEAVES_HPP
#define B_LEAVES_HPP

#include "block.hpp"

namespace Blocks{
class Leaves : public Block {
public:
  Leaves();
  block_data getBlockData();
};
}
#endif
