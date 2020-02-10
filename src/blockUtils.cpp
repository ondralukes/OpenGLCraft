#include "blockUtils.hpp"
#include "structs.hpp"
#include "chunk.hpp"
#include <cstdio>
#include <math.h>

bool isBlock(intvec3 pos){
  intvec3 chunkPos(
    floor(pos.x/(float)CHUNK_SIZE),
    floor(pos.y/(float)CHUNK_SIZE),
    floor(pos.z/(float)CHUNK_SIZE)
  );
  Chunk * ch = Chunk::getChunk(chunkPos);
  if(ch == NULL) return false;
  intvec3 relPos(
    pos.x - chunkPos.x*CHUNK_SIZE,
    pos.y - chunkPos.y*CHUNK_SIZE,
    pos.z - chunkPos.z*CHUNK_SIZE
  );

  return ch->blocks[relPos.x][relPos.y][relPos.z] != NULL;
}

void destroyBlock(intvec3 pos, double time, Blocks::Block * usedTool, bool update){
  intvec3 chunkPos(
    floor(pos.x/(float)CHUNK_SIZE),
    floor(pos.y/(float)CHUNK_SIZE),
    floor(pos.z/(float)CHUNK_SIZE)
  );
  Chunk * ch = Chunk::getChunk(chunkPos);
  if(ch == NULL) return;
  intvec3 relPos(
    pos.x - chunkPos.x*CHUNK_SIZE,
    pos.y - chunkPos.y*CHUNK_SIZE,
    pos.z - chunkPos.z*CHUNK_SIZE
  );
  if(ch->blocks[relPos.x][relPos.y][relPos.z] == NULL) return;
  float hardness = ch->blocks[relPos.x][relPos.y][relPos.z]->getHardness(usedTool);
  ch->blocks[relPos.x][relPos.y][relPos.z]->damageLevel = floor(time / hardness);
  if(ch->blocks[relPos.x][relPos.y][relPos.z]->damageLevel > 8){
    delete ch->blocks[relPos.x][relPos.y][relPos.z];
    ch->blocks[relPos.x][relPos.y][relPos.z] = NULL;
    if(usedTool != NULL){
      bool destroyed = usedTool->usedAsTool();
      if(destroyed){
        Inventory::destroySelectedBlock();
        GUI::refresh();
      }
    }
  }
  if(update)ch->update();
}

void removeBlock(intvec3 pos, bool update){
  intvec3 chunkPos(
    floor(pos.x/(float)CHUNK_SIZE),
    floor(pos.y/(float)CHUNK_SIZE),
    floor(pos.z/(float)CHUNK_SIZE)
  );
  Chunk * ch = Chunk::getChunk(chunkPos);
  if(ch == NULL) return;
  intvec3 relPos(
    pos.x - chunkPos.x*CHUNK_SIZE,
    pos.y - chunkPos.y*CHUNK_SIZE,
    pos.z - chunkPos.z*CHUNK_SIZE
  );
  if(ch->blocks[relPos.x][relPos.y][relPos.z] == NULL) return;
  ch->blocks[relPos.x][relPos.y][relPos.z]->doDrop = false;
  delete ch->blocks[relPos.x][relPos.y][relPos.z];
  ch->blocks[relPos.x][relPos.y][relPos.z] = NULL;
  if(update)ch->update();
}

void addBlock(intvec3 pos, Blocks::Block * bl, bool update){
  intvec3 chunkPos(
    floor(pos.x/(float)CHUNK_SIZE),
    floor(pos.y/(float)CHUNK_SIZE),
    floor(pos.z/(float)CHUNK_SIZE)
  );
  Chunk * ch = Chunk::getChunk(chunkPos);
  if(ch == NULL){
    Chunk::setChunk(chunkPos, new Chunk(chunkPos), update);
    ch = Chunk::getChunk(chunkPos);
  }
  intvec3 relPos(
    pos.x - chunkPos.x*CHUNK_SIZE,
    pos.y - chunkPos.y*CHUNK_SIZE,
    pos.z - chunkPos.z*CHUNK_SIZE
  );
  ch->blocks[relPos.x][relPos.y][relPos.z] = bl;
  if(update)ch->update();
}

Blocks::Block * getBlock(intvec3 pos){
  intvec3 chunkPos(
    floor(pos.x/(float)CHUNK_SIZE),
    floor(pos.y/(float)CHUNK_SIZE),
    floor(pos.z/(float)CHUNK_SIZE)
  );
  Chunk * ch = Chunk::getChunk(chunkPos);
  if(ch == NULL) return NULL;
  intvec3 relPos(
    pos.x - chunkPos.x*CHUNK_SIZE,
    pos.y - chunkPos.y*CHUNK_SIZE,
    pos.z - chunkPos.z*CHUNK_SIZE
  );
  if(ch->blocks[relPos.x][relPos.y][relPos.z] == NULL) return NULL;
  return ch->blocks[relPos.x][relPos.y][relPos.z];
}
