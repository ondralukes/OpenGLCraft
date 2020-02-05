#ifndef B_STICK_HPP
#define B_STICK_HPP

#include "block.hpp"

namespace Blocks{
class Stick : public Block {
public:
  Stick();
  block_data getBlockData();
};
}
#endif
