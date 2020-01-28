#include "dirt.hpp"

using namespace Blocks;

Dirt::Dirt() : Block("textures/dirt.dds"){
    type = DIRT;
    hardness = 0.1f;
}

block_data
Dirt::getBlockData(){
  block_data data;
  data.type = DIRT;
  return data;
}
