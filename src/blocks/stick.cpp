#include "stick.hpp"

using namespace Blocks;

Stick::Stick() : Block("textures/stick.dds"){
    data.type = STICK;
    hardness = 0.1f;
    drawFlat = true;
    fuelValue = 0.1f;
}
