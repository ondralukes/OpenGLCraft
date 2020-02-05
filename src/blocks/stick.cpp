#include "stick.hpp"

using namespace Blocks;

Stick::Stick() : Block("textures/stick.dds"){
    type = STICK;
    hardness = 0.1f;
}

block_data
Stick::getBlockData(){
  block_data data;
  data.type = STICK;
  return data;
}
