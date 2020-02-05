#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <GL/glew.h>
#include "resourceManager.hpp"
#include "structs.hpp"

namespace Blocks{
#define BLOCK_DATA_SIZE sizeof(block_type) + sizeof(size_t)
  enum block_type : uint16_t
  {
    NONE = 0,
    GRASS = 1,
    STONE = 2,
    DIRT = 3,
    WOOD = 4,
    LEAVES = 5,
    STICK = 6,
    WOODEN_PICKAXE = 7,
    STONE_PICKAXE = 8
  };

  enum tool_type : uint16_t
  {
    NO_TOOL = 0,
    PICKAXE = 1
  };

  struct block_data {
    block_type type;
    char data[BLOCK_DATA_SIZE];
  };
  class Block {
  public:
    Block(const char * texpath);
    ~Block();
    static Block * decodeBlock(block_data data, intvec3 pos, GLuint mvpid);
    static GLuint getTextureFor(block_type type);
    static bool canPlace(block_type type);
    GLuint textureID = 100;
    GLuint mvpID;
    int damageLevel = 0;
    intvec3 pos;
    block_type type;
    virtual block_data getBlockData() = 0;
    float getHardness(Block * b);
    tool_type toolType = NO_TOOL;
    float toolLevel = 0.0f;
    bool doDrop = true;
  protected:
    float hardness;
    float pickaxeEff = 0.0f;
  };
}
#endif
