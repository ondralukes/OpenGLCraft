#include "worldGenerator.hpp"

#include "chunk.hpp"
#include "structs.hpp"
#include "block.hpp"

void
WorldGenerator::generate(glm::vec3 pos, float deltaTime){
  for(int chx = floor(pos.x/CHUNK_SIZE)-6;chx<floor(pos.x/CHUNK_SIZE)+6;chx++){
    for(int chz = floor(pos.z/CHUNK_SIZE)-6;chz<floor(pos.z/CHUNK_SIZE)+6;chz++){

      intvec3 chunkPos(
        chx,
        -1,
        chz
      );
      Chunk * ch = Chunk::getChunk(chunkPos);
      if(ch==NULL){
        Chunk::setChunk(chunkPos,new Chunk(chunkPos));
        ch = Chunk::getChunk(chunkPos);
        if(!ch->isLoaded){
          for(int x = 0;x<CHUNK_SIZE;x++){
            for(int y = -8*CHUNK_SIZE;y<0;y++){
              for(int z = 0;z<CHUNK_SIZE;z++){
                addBlock(intvec3(chunkPos.x*CHUNK_SIZE+x,y,chunkPos.z*CHUNK_SIZE+z),false);
              }
            }
          }
          for(int y = -8;y<4;y++){
            ch = Chunk::getChunk(intvec3(chunkPos.x,y,chunkPos.z));
            if(ch==NULL){
              Chunk::setChunk(intvec3(chunkPos.x,y,chunkPos.z), new Chunk(intvec3(chunkPos.x,y,chunkPos.z)));
              ch = Chunk::getChunk(intvec3(chunkPos.x,y,chunkPos.z));
            }
            ch->update(true);
          }
        } else {
          for(int y = -8;y<4;y++){
            ch = Chunk::getChunk(intvec3(chunkPos.x,y,chunkPos.z));
            if(ch==NULL){
              Chunk::setChunk(intvec3(chunkPos.x,y,chunkPos.z), new Chunk(intvec3(chunkPos.x,y,chunkPos.z)));
              ch = Chunk::getChunk(intvec3(chunkPos.x,y,chunkPos.z));
            }
            ch->update(false);
          }
        }
      }
    }
  }
}
