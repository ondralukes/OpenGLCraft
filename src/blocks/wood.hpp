#ifndef B_WOOD_HPP
#define B_WOOD_HPP

#include "block.hpp"

namespace Blocks{
class Wood : public Block {
public:
  Wood();
  block_data getBlockData();
};
}
#endif
