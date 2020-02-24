#ifndef BLOCK_UTILS_HPP
#define BLOCK_UTILS_HPP

#include "structs.hpp"
#include "chunk.hpp"
#include "blocks/blocks.hpp"

class Chunk;

bool isBlock(intvec3 pos);
void destroyBlock(intvec3 pos, double time, Blocks::Block * usedTool, bool update = true);
void addBlock(intvec3 pos, Blocks::Block * bl, bool update = true);
void removeBlock(intvec3 pos, bool update = true);
Chunk * chunkOf(intvec3 pos);
void saveBlock(intvec3 pos);
int getUnderSky(int x, int z);
Blocks::Block * getBlock(intvec3 pos);

#endif
