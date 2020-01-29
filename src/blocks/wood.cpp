#include "wood.hpp"

using namespace Blocks;

Wood::Wood() : Block("textures/wood.dds"){
  type = WOOD;
  hardness = 0.2f;
}

block_data
Wood::getBlockData(){
  block_data data;
  data.type = WOOD;
  return data;
}
