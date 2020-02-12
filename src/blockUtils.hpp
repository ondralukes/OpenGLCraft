#ifndef BLOCK_UTILS_HPP
#define BLOCK_UTILS_HPP

#include "structs.hpp"
#include "blocks/blocks.hpp"

bool isBlock(intvec3 pos);
void destroyBlock(intvec3 pos, double time, Blocks::Block * usedTool, bool update = true);
void addBlock(intvec3 pos, Blocks::Block * bl, bool update = true);
void removeBlock(intvec3 pos, bool update = true);
void saveBlock(intvec3 pos);
Blocks::Block * getBlock(intvec3 pos);

#endif
