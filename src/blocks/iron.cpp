#include "iron.hpp"

using namespace Blocks;

Iron::Iron() : Block("textures/iron.dds"){
    data.type = IRON;
    drawFlat = true;
}
