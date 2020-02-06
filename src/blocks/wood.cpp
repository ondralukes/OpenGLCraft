#include "wood.hpp"

using namespace Blocks;

Wood::Wood() : Block("textures/wood.dds"){
  data.type = WOOD;
  hardness = 0.2f;
}

void
Wood::load(){
}
