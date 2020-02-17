#include "fire.hpp"

using namespace Blocks;

Fire::Fire() : Block("textures/fire.dds"){
  data.type = FIRE;
  hardness = 0.025f;
  isLightSource = true;
}
