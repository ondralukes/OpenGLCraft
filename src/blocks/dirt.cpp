#include "dirt.hpp"

using namespace Blocks;

Dirt::Dirt() : Block("textures/dirt.bmp"){
}

block_data
Dirt::getBlockData(){
  block_data data;
  data.type = DIRT;
  return data;
}
