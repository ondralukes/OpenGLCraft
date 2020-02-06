#include "pickaxe.hpp"

using namespace Blocks;

WoodenPickaxe::WoodenPickaxe() : Block("textures/woodenPickaxe.dds"){
  data.type = WOODEN_PICKAXE;
  toolType = PICKAXE;
  toolLevel = 1.0f;
  hp = 25;
  maxStack = 1;
  drawFlat = true;
}

void
WoodenPickaxe::save(){
  if(data.dataPos == 0){
    data.dataPos = SaveManager::main->allocateBlockData(sizeof(int));
  }
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  fwrite(&hp, sizeof(int), 1, fp);
}

void
WoodenPickaxe::load(){
  if(data.dataPos == 0) return;
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  fread(&hp, sizeof(int), 1, fp);
}

bool
WoodenPickaxe::usedAsTool(){
  hp--;
  if(hp == 0) return true;
  save();
  return false;
}

float
WoodenPickaxe::getHP(){
  return hp/25.0f;
}

StonePickaxe::StonePickaxe() : Block("textures/stonePickaxe.dds"){
  data.type = STONE_PICKAXE;
  toolType = PICKAXE;
  toolLevel = 2.0f;
  hp = 75;
  maxStack = 1;
  drawFlat = true;
}

void
StonePickaxe::save(){
  if(data.dataPos == 0){
    data.dataPos = SaveManager::main->allocateBlockData(sizeof(int));
  }
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  fwrite(&hp, sizeof(int), 1, fp);
}

void
StonePickaxe::load(){
  if(data.dataPos == 0) return;
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  fread(&hp, sizeof(int), 1, fp);
}

bool
StonePickaxe::usedAsTool(){
  hp--;
  if(hp == 0) return true;
  save();
  return false;
}

float
StonePickaxe::getHP(){
  return hp/75.0f;
}
