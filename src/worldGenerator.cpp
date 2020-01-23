#include "worldGenerator.hpp"

#include "chunk.hpp"
#include "structs.hpp"
#include "blockUtils.hpp"
#include "blocks/blocks.hpp"

void
WorldGenerator::generate(glm::vec3 pos, float deltaTime){
  for(int tx = -1;tx<=1;tx++){
    for(int tz = -1;tz<=1;tz++){
      intvec3 terrainPartPos(
        floor(pos.x/CHUNK_SIZE/4)*4+tx*4,
        -1,
        floor(pos.z/CHUNK_SIZE/4)*4+tz*4
      );
      int terrStartX = floor(pos.x/CHUNK_SIZE/4)*4+tx*4;
      int terrStartZ = floor(pos.z/CHUNK_SIZE/4)*4+tz*4;
      Chunk * ch = Chunk::getChunk(terrainPartPos);
      if(ch!=NULL)continue;
      Chunk::setChunk(terrainPartPos,new Chunk(terrainPartPos));
      ch = Chunk::getChunk(terrainPartPos);
      bool isLoaded = ch->isLoaded;
      int ** map = new int*[CHUNK_SIZE*4];
      for(int i = 0;i<CHUNK_SIZE*4;i++){
        map[i] = new int[CHUNK_SIZE*4];
        for(int j =0;j<CHUNK_SIZE*4;j++){
          map[i][j] = 0;
        }
      }
      if(!isLoaded)  generateTerrain(map,CHUNK_SIZE*4,CHUNK_SIZE*4);
      for(int chx = terrStartX;chx<terrStartX+4;chx++){
        for(int chz = terrStartZ;chz<terrStartZ+4;chz++){
          intvec3 chunkPos(
            chx,
            -1,
            chz
          );
          if(Chunk::getChunk(chunkPos) == NULL) Chunk::setChunk(chunkPos,new Chunk(chunkPos));
          ch = Chunk::getChunk(chunkPos);
          if(!isLoaded){
            for(int x = 0;x<CHUNK_SIZE;x++){
              for(int z = 0;z<CHUNK_SIZE;z++){
                int terrX = x+chx*CHUNK_SIZE - terrStartX*CHUNK_SIZE;
                int terrY = z+chz*CHUNK_SIZE - terrStartZ*CHUNK_SIZE;
                int height = map[terrX][terrY];
                int dirtHeight = height - (rand()%5+5);
                for(int y = -4*CHUNK_SIZE;y<=height;y++){
                  Blocks::Block * bl;
                  if(y==height){
                    bl = new Blocks::Grass();
                  } else if(y>dirtHeight){
                    bl = new Blocks::Dirt();
                  } else {
                    bl = new Blocks::Stone();
                  }
                  addBlock(intvec3(chunkPos.x*CHUNK_SIZE+x,y,chunkPos.z*CHUNK_SIZE+z), bl, false);
                }
              }
            }
            for(int y = -8;y<8;y++){
              ch = Chunk::getChunk(intvec3(chunkPos.x,y,chunkPos.z));
              if(ch==NULL){
                Chunk::setChunk(intvec3(chunkPos.x,y,chunkPos.z), new Chunk(intvec3(chunkPos.x,y,chunkPos.z)));
                ch = Chunk::getChunk(intvec3(chunkPos.x,y,chunkPos.z));
              }
              ch->update(true);
            }
          } else {
            for(int y = -8;y<8;y++){
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

      for(int i = 0;i<CHUNK_SIZE*4;i++){
        free(map[i]);
      }
      free(map);
    }
  }
}

void
WorldGenerator::generateTerrain(int ** map, int xSize, int ySize){
  int peakCount = xSize*ySize/1000;
  float radius = (xSize+ySize)/2.0f;
  for(int i = 0;i<peakCount;i++){
    float height = ((rand()%2000) - 1000)/50.0f;
    int px = rand()%xSize;
    int py = rand()%ySize;
    for(int x = 0;x<xSize;x++){
      for(int y = 0;y<ySize;y++){
        float d = (radius-sqrt((px-x)*(px-x)+(py-y)*(py-y)))/radius;
        if(d<0) d = 0;
        float h = d*height;
        map[x][y] += (int) round(h);
        if(map[x][y] > 30) map[x][y] = 30;
        if(map[x][y] < -30) map[x][y] = -30;
      }
    }
  }
}
