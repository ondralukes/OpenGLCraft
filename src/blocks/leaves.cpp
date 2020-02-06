#include "leaves.hpp"

using namespace Blocks;

Leaves::Leaves() : Block("textures/leaves.dds"){
  data.type = LEAVES;
  hardness = 0.025f;
}
