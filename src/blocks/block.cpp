#include "block.hpp"

#include "blocks.hpp"
#include "droppedBlock.hpp"

using namespace Blocks;

Block::Block(const char * texpath){
  textureID = ResourceManager::getTexture(texpath);
  data.dataPos = 0;
}

Block::~Block(){
  if(!doDrop) return;
  glm::vec3 p(pos.x,pos.y,pos.z);
  Block * droppedBlock = decodeBlock(data, pos, mvpID);
  DroppedBlock * drop = new DroppedBlock(mvpID, droppedBlock, p);
  glm::vec3 vel(
    ((rand()%40) - 20)* 0.01f,
    1.5f,
    ((rand()%40) - 20)* 0.01f
  );
  drop->setVelocity(vel);
}

Block *
Block::decodeBlock(block_data data, intvec3 pos, GLuint mvpid){
  Block * bl;
  switch (data.type) {
    case GRASS:
      bl = (Block *)new Grass();
      break;
    case STONE:
      bl = (Block *)new Stone();
      break;
    case DIRT:
      bl = (Block *)new Dirt();
      break;
    case WOOD:
      bl = (Block *)new Wood();
      break;
    case LEAVES:
      bl = (Block *)new Leaves();
      break;
    case STICK:
      bl = (Block *)new Stick();
      break;
    case WOODEN_PICKAXE:
      bl = (Block *)new WoodenPickaxe();
      break;
    case STONE_PICKAXE:
      bl = (Block *)new StonePickaxe();
      break;
    default:
      bl = NULL;
      break;
  }
  if(bl!=NULL){
    bl->data = data;
    bl->pos = pos;
    bl->mvpID = mvpid;
    bl->load();
  }
  return bl;
}

GLuint
Block::getTextureFor(block_type type){
  if(type == GRASS){
    return ResourceManager::getTexture("textures/grass.dds");
  }
  if(type == STONE){
    return ResourceManager::getTexture("textures/stone.dds");
  }
  if(type == DIRT){
    return ResourceManager::getTexture("textures/dirt.dds");
  }
  if(type == WOOD){
    return ResourceManager::getTexture("textures/wood.dds");
  }
  if(type == LEAVES){
    return ResourceManager::getTexture("textures/leaves.dds");
  }
  if(type == STICK){
    return ResourceManager::getTexture("textures/stick.dds");
  }
  if(type == WOODEN_PICKAXE){
    return ResourceManager::getTexture("textures/woodenPickaxe.dds");
  }
  if(type == STONE_PICKAXE){
    return ResourceManager::getTexture("textures/stonePickaxe.dds");
  }
  printf("No texture for block!\n");
}

bool
Block::canPlace(block_type type){
  if(type == STICK) return false;
  if(type == WOODEN_PICKAXE || type == STONE_PICKAXE) return false;
  return true;
}

float
Block::getHardness(Block * b){
  if(b==NULL) return hardness;
  if(b->toolType == PICKAXE){
    return hardness / (1.0f + b->toolLevel * pickaxeEff);
  }
  return hardness;
}

block_data
Block::getBlockData(){
  return data;
}

block_type
Block::getType(){
  return data.type;
}

void
Block::load(){

}

void
Block::save(){

}

bool
Block::usedAsTool(){
  return false;
}

float
Block::getHP(){
  return 1.0f;
}
