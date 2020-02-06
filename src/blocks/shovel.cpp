#include "shovel.hpp"

using namespace Blocks;

WoodenShovel::WoodenShovel() : Block("textures/woodenShovel.dds"){
  data.type = WOODEN_SHOVEL;
  toolType = SHOVEL;
  toolLevel = 1.0f;
  hp = 50;
  maxStack = 1;
  drawFlat = true;
}

void
WoodenShovel::save(){
  if(data.dataPos == 0){
    data.dataPos = SaveManager::main->allocateBlockData(sizeof(int));
  }
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  fwrite(&hp, sizeof(int), 1, fp);
}

void
WoodenShovel::load(){
  if(data.dataPos == 0) return;
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  fread(&hp, sizeof(int), 1, fp);
}

bool
WoodenShovel::usedAsTool(){
  hp--;
  if(hp == 0) return true;
  save();
  return false;
}

float
WoodenShovel::getHP(){
  return hp/50.0f;
}

StoneShovel::StoneShovel() : Block("textures/stoneShovel.dds"){
  data.type = STONE_SHOVEL;
  toolType = SHOVEL;
  toolLevel = 2.0f;
  hp = 150;
  maxStack = 1;
  drawFlat = true;
}

void
StoneShovel::save(){
  if(data.dataPos == 0){
    data.dataPos = SaveManager::main->allocateBlockData(sizeof(int));
  }
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  fwrite(&hp, sizeof(int), 1, fp);
}

void
StoneShovel::load(){
  if(data.dataPos == 0) return;
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  fread(&hp, sizeof(int), 1, fp);
}

bool
StoneShovel::usedAsTool(){
  hp--;
  if(hp == 0) return true;
  save();
  return false;
}

float
StoneShovel::getHP(){
  return hp/150.0f;
}
