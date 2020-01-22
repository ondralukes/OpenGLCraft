#ifndef BLOCK_UTILS_HPP
#define BLOCK_UTILS_HPP

#include "structs.hpp"
#include "blocks/blocks.hpp"

bool isBlock(intvec3 pos);
void removeBlock(intvec3 pos, bool update = true);
void addBlock(intvec3 pos, Blocks::Block * bl, bool update = true);

#endif
