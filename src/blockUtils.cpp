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
    Chunk::setChunk(chunkPos, new Chunk(chunkPos));
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
