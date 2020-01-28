#include "stone.hpp"

using namespace Blocks;

Stone::Stone() : Block("textures/stone.dds"){
  type = STONE;
  hardness = 0.5f;
}

block_data
Stone::getBlockData(){
  block_data data;
  data.type = STONE;
  return data;
}
