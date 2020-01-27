#include "grass.hpp"

using namespace Blocks;

Grass::Grass() : Block("textures/grass.dds"){
  type = GRASS;
}

block_data
Grass::getBlockData(){
  block_data data;
  data.type = GRASS;
  return data;
}
