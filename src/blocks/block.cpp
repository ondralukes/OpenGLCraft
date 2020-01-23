#include "block.hpp"

#include "blocks.hpp"

using namespace Blocks;

Block::Block(const char * texpath){
  textureID = ResourceManager::getTexture(texpath);
}

Block *
Block::decodeBlock(block_data data){
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
  return bl;
}
