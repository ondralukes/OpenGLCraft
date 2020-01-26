#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <GL/glew.h>
#include <unistd.h>
#include "resourceManager.hpp"
#include "structs.hpp"

namespace Blocks{
#define BLOCK_DATA_SIZE sizeof(block_type) + sizeof(size_t)
  enum block_type : uint16_t
  {
    NONE = 0,
    GRASS = 1,
    STONE = 2,
    DIRT = 3
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
    GLuint textureID;
    GLuint mvpID;
    intvec3 pos;
    virtual block_data getBlockData() = 0;
  };
}
#endif
