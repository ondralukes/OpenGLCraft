#include "stone.hpp"

using namespace Blocks;

Stone::Stone() : Block("textures/stone.dds"){
  data.type = STONE;
  hardness = 0.5f;
  pickaxeEff = 1.0f;
}
