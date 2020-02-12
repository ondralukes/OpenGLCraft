#include "chest.hpp"

using namespace Blocks;

Chest::Chest() : Block("textures/chest.dds"){
    data.type = CHEST;
    hardness = 0.1f;
    content = (inventory_item *) malloc((sizeof(block_data) + sizeof(int)) * 24);
    memset(content, 0, sizeof(inventory_item) * 24);
}

void
Chest::destroy(){
  if(doDrop){
    //Drop the content and empty the chest
    for(int i = 0;i<24;i++){
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
    save();
  }
  free(content);
}

bool
Chest::rightClick(){
  GUI::enterGUI(GUI_CHEST, this);
  return true;
}

void
Chest::setContent(int index, inventory_item data){
  content[index] = data;
}

inventory_item
Chest::getContent(int index){
  return content[index];
}

void
Chest::save(){
  if(data.dataPos == 0){
    data.dataPos = SaveManager::main->allocateBlockData((sizeof(block_data) + sizeof(int)) * 24);
    saveBlock(pos);
  }
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  for(int i = 0;i < 24;i++){
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
Chest::load(){
  if(data.dataPos == 0) return;
  FILE * fp = SaveManager::main->getBlockDatafp();
  fseek(fp, data.dataPos, SEEK_SET);
  for(int i = 0;i < 24;i++){
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
  fseek(fp, data.dataPos, SEEK_SET);
  char buffer[(sizeof(block_data) + sizeof(int)) * 24];
  long chsum = 0;
  fread(buffer, (sizeof(block_data) + sizeof(int)) * 24,1,fp);
  for(int i =0;i<(sizeof(block_data) + sizeof(int)) * 24;i++){
    chsum += buffer[i] * i;
  }
}
