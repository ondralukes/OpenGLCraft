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
  printf("Blek\n");
}
