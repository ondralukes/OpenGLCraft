#include "ironOre.hpp"

using namespace Blocks;

IronOre::IronOre() : Block("textures/ironOre.dds"){
  data.type = IRON_ORE;
  hardness = 0.5f;
  pickaxeEff = 1.0f;
  meltsTo = IRON;
  meltFuel = 0.25f;
}
