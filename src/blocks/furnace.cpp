#include "furnace.hpp"

using namespace Blocks;

Furnace::Furnace() : Block("textures/furnace.dds"){
  data.type = FURNACE;
  hardness = 0.25f;
  pickaxeEff = 1.0f;
  maxStack = 1;
  content = (inventory_item *) malloc((sizeof(block_data) + sizeof(int)) * 3);
  memset(content, 0, sizeof(inventory_item) * 3);
  startTime = glfwGetTime();
}

void
Furnace::destroy(){
  if(doDrop){
    //Drop the content and empty the chest
    for(int i = 0;i<3;i++){
      if(content[i].block != NULL && content[i].count != 0){
        for(int j = 0;j < content[i].count;j++){
          Block * droppedBlock = decodeBlock(content[i].block->getBlockData(), pos, GUI::mvpID);
          DroppedBlock * drop = new DroppedBlock(GUI::mvpID, droppedBlock, glm::vec3(pos.x, pos.y, pos.z));
          glm::vec3 vel(
            ((rand()%40) - 20)* 0.01f,
            1.5f,
            ((rand()%40) - 20)* 0.01f
          );
          drop->setVelocity(vel);
        }
        delete content[i].block;
      }
      content[i].block = NULL;
      content[i].count = NULL;
    }
    fuel = 0.0f;
    blockFuelBurnt = 0.0f;
    burning = false;
    save();
  }
  free(content);
}

bool
Furnace::rightClick(){
  GUI::enterGUI(GUI_FURNACE, this);
  return true;
}

void
Furnace::save(){
  if(data.dataPos == 0){
    data.dataPos = SaveManager::main->allocateBlockData((sizeof(block_data) + sizeof(int)) * 3 + 2*sizeof(float));
    saveBlock(pos);
  }
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  fwrite(&fuel, sizeof(float), 1, fp);
  fwrite(&blockFuelBurnt, sizeof(float), 1, fp);
  for(int i = 0;i < 3;i++){
    block_data data;
    int count = content[i].count;
    if(content[i].block != NULL){
      data = content[i].block->getBlockData();
    } else {
      data.type = NONE;
      data.dataPos = 0;
      count = 0;
    }
    fwrite(&data, sizeof(block_data), 1, fp);
    fwrite(&count, sizeof(int), 1, fp);
  }
}

void
Furnace::load(){
  if(data.dataPos == 0) return;
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  fread(&fuel, sizeof(float), 1, fp);
  fread(&blockFuelBurnt, sizeof(float), 1, fp);
  for(int i = 0;i < 3;i++){
    block_data data;
    data.type = NONE;
    int count = 0;
    fread(&data, sizeof(block_data), 1, fp);
    fread(&count, sizeof(int), 1, fp);
    if(data.type != NONE){
      content[i].block = Block::decodeBlock(data, intvec3(0,0,0), GUI::mvpID);
      content[i].count = count;
    }
  }
  if(content[0].block != NULL){
    if(content[0].block->meltsTo != NONE){
      if(content[2].block != NULL){
        if(content[2].block->getType() == content[0].block->meltsTo){
          burning = true;
        }
      } else {
        burning = true;
      }
    }
  }
}

void
Furnace::update(){
  if(burning){
    double t = glfwGetTime();
    float d = (t-startTime)*speed;
    if(d > fuel) d = fuel;
    blockFuelBurnt += d;
    fuel -= d;
    startTime += d/speed;
    if(fuel == 0.0f) startTime = glfwGetTime();
  } else{
    blockFuelBurnt = 0.0f;
    startTime = glfwGetTime();
  }
}
