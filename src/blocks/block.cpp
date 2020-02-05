#include "block.hpp"

#include "blocks.hpp"
#include "droppedBlock.hpp"

using namespace Blocks;

Block::Block(const char * texpath){
  textureID = ResourceManager::getTexture(texpath);
}

Block::~Block(){
  glm::vec3 p(pos.x,pos.y,pos.z);
  DroppedBlock * drop = new DroppedBlock(mvpID, type, p);
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
    default:
      bl = NULL;
      break;
  }
  if(bl!=NULL){
    bl->pos = pos;
    bl->mvpID = mvpid;
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
  printf("No texture for block!\n");
}

bool
Block::canPlace(block_type type){
  if(type == STICK) return false;
  return true;
}
