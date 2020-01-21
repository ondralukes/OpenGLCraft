#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "structs.hpp"

bool isBlock(intvec3 pos);
void removeBlock(intvec3 pos, bool update = true);
void addBlock(intvec3 pos, bool update = true);

#endif
