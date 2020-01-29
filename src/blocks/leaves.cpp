#include "leaves.hpp"

using namespace Blocks;

Leaves::Leaves() : Block("textures/leaves.dds"){
  type = LEAVES;
  hardness = 0.025f;
}

block_data
Leaves::getBlockData(){
  block_data data;
  data.type = LEAVES;
  return data;
}
