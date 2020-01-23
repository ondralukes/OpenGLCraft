#include "dirt.hpp"

using namespace Blocks;

Dirt::Dirt() : Block("textures/dirt.dds"){
}

block_data
Dirt::getBlockData(){
  block_data data;
  data.type = DIRT;
  return data;
}
