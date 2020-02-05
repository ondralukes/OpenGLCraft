#include "pickaxe.hpp"

using namespace Blocks;

WoodenPickaxe::WoodenPickaxe() : Block("textures/woodenPickaxe.dds"){
  type = WOODEN_PICKAXE;
  toolType = PICKAXE;
  toolLevel = 1.0f;
}

block_data
WoodenPickaxe::getBlockData(){
  block_data data;
  data.type = WOODEN_PICKAXE;
  return data;
}

StonePickaxe::StonePickaxe() : Block("textures/stonePickaxe.dds"){
  type = STONE_PICKAXE;
  toolType = PICKAXE;
  toolLevel = 2.0f;
}

block_data
StonePickaxe::getBlockData(){
  block_data data;
  data.type = STONE_PICKAXE;
  return data;
}
