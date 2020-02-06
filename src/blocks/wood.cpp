#include "wood.hpp"

using namespace Blocks;

Wood::Wood() : Block("textures/wood.dds"){
  data.type = WOOD;
  hardness = 0.3f;
  pickaxeEff = 0.1f;
  axeEff = 1.0f;
}

void
Wood::load(){
}
