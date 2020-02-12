#include "chest.hpp"

using namespace Blocks;

Chest::Chest() : Block("textures/chest.dds"){
    data.type = CHEST;
    hardness = 0.1f;
}
