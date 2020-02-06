#include "axe.hpp"

using namespace Blocks;

WoodenAxe::WoodenAxe() : Block("textures/woodenAxe.dds"){
  data.type = WOODEN_AXE;
  toolType = AXE;
  toolLevel = 1.0f;
  hp = 25;
  maxStack = 1;
  drawFlat = true;
}

void
WoodenAxe::save(){
  if(data.dataPos == 0){
    data.dataPos = SaveManager::main->allocateBlockData(sizeof(int));
  }
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  fwrite(&hp, sizeof(int), 1, fp);
}

void
WoodenAxe::load(){
  if(data.dataPos == 0) return;
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  fread(&hp, sizeof(int), 1, fp);
}

bool
WoodenAxe::usedAsTool(){
  hp--;
  if(hp == 0) return true;
  save();
  return false;
}

float
WoodenAxe::getHP(){
  return hp/25.0f;
}

StoneAxe::StoneAxe() : Block("textures/stoneAxe.dds"){
  data.type = STONE_AXE;
  toolType = AXE;
  toolLevel = 2.0f;
  hp = 75;
  maxStack = 1;
  drawFlat = true;
}

void
StoneAxe::save(){
  if(data.dataPos == 0){
    data.dataPos = SaveManager::main->allocateBlockData(sizeof(int));
  }
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  fwrite(&hp, sizeof(int), 1, fp);
}

void
StoneAxe::load(){
  if(data.dataPos == 0) return;
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  fread(&hp, sizeof(int), 1, fp);
}

bool
StoneAxe::usedAsTool(){
  hp--;
  if(hp == 0) return true;
  save();
  return false;
}

float
StoneAxe::getHP(){
  return hp/75.0f;
}
