#include "grass.hpp"

using namespace Blocks;

Grass::Grass() : Block("textures/grass.dds"){
  data.type = GRASS;
  hardness = 0.05f;
}
