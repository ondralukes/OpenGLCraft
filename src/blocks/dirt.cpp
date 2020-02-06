#include "dirt.hpp"

using namespace Blocks;

Dirt::Dirt() : Block("textures/dirt.dds"){
    data.type = DIRT;
    hardness = 0.1f;
}
