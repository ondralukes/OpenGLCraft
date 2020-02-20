#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <GL/glew.h>
#include "resourceManager.hpp"
#include "structs.hpp"

namespace Blocks{
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
    STONE_PICKAXE = 8,
    WOODEN_AXE = 9,
    STONE_AXE = 10,
    WOODEN_SHOVEL = 11,
    STONE_SHOVEL = 12,
    CHEST = 13,
    FIRE = 14,
    IRON_ORE = 15,
    FURNACE = 16,
    IRON = 17,

    ___BLOCKS_END___ = 18
  };

  enum tool_type : uint16_t
  {
    NO_TOOL = 0,
    PICKAXE = 1,
    AXE = 2,
    SHOVEL = 3
  };

  struct block_data {
    block_type type;
    size_t dataPos;
  };

  class Block {
  public:
    Block(const char * texpath);
    ~Block();
    static void initBlockTextures();
    static Block * decodeBlock(block_data data, intvec3 pos, GLuint mvpid);
    static GLuint getTextureFor(block_type type);
    static bool canPlace(block_type type);
    virtual void save();
    virtual void load();
    virtual bool usedAsTool();
    virtual float getHP();
    virtual bool rightClick();
    virtual void destroy();
    GLuint textureID = 100;
    GLuint mvpID;
    int damageLevel = 0;
    float fuelValue = 0.0f;
    int maxStack = 64;
    bool drawFlat = false;
    intvec3 pos;
    block_data getBlockData();
    block_type getType();
    float getHardness(Block * b);
    tool_type toolType = NO_TOOL;
    float toolLevel = 0.0f;
    bool isLightSource = false;
    bool doDrop = true;
    block_type meltsTo = NONE;
    float meltFuel = 0.0f;
  protected:
    float hardness;
    float pickaxeEff = 0.0f;
    float axeEff = 0.0f;
    float shovelEff = 0.0f;
    block_data data;
  };
}
#endif
