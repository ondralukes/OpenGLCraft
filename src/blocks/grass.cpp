#include "grass.hpp"

using namespace Blocks;

Grass::Grass() : Block("textures/grass.dds"){
  type = GRASS;
  hardness = 0.05f;
}

block_data
Grass::getBlockData(){
  block_data data;
  data.type = GRASS;
  return data;
}
