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
      Chunk::setChunk(terrainPartPos,new Chunk(terrainPartPos), false);
      ch = Chunk::getChunk(terrainPartPos);
      bool isLoaded = ch->isLoaded;
      int ** map = new int*[CHUNK_SIZE*4];
      for(int i = 0;i<CHUNK_SIZE*4;i++){
        map[i] = new int[CHUNK_SIZE*4];
        for(int j =0;j<CHUNK_SIZE*4;j++){
          map[i][j] = 0;
        }
      }
      if(!isLoaded)  generateTerrain(map,terrStartX, terrStartZ, CHUNK_SIZE*4,CHUNK_SIZE*4);
      for(int chx = terrStartX;chx<terrStartX+4;chx++){
        for(int chz = terrStartZ;chz<terrStartZ+4;chz++){
          intvec3 chunkPos(
            chx,
            -1,
            chz
          );
          if(Chunk::getChunk(chunkPos) == NULL) Chunk::setChunk(chunkPos,new Chunk(chunkPos), false);
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
                  bl->pos = intvec3(chunkPos.x*CHUNK_SIZE+x,y,chunkPos.z*CHUNK_SIZE+z);
                  addBlock(intvec3(chunkPos.x*CHUNK_SIZE+x,y,chunkPos.z*CHUNK_SIZE+z), bl, false);
                }
              }
            }
            placeTrees(chx * CHUNK_SIZE, chz * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, 3);
            Chunk::saveHeader();
            for(int y = -8;y<8;y++){
              ch = Chunk::getChunk(intvec3(chunkPos.x,y,chunkPos.z));
              if(ch==NULL){
                Chunk::setChunk(intvec3(chunkPos.x,y,chunkPos.z), new Chunk(intvec3(chunkPos.x,y,chunkPos.z)), false);
                ch = Chunk::getChunk(intvec3(chunkPos.x,y,chunkPos.z));
              }
              ch->update(true);
            }
          } else {
            for(int y = -8;y<8;y++){
              ch = Chunk::getChunk(intvec3(chunkPos.x,y,chunkPos.z));
              if(ch==NULL){
                Chunk::setChunk(intvec3(chunkPos.x,y,chunkPos.z), new Chunk(intvec3(chunkPos.x,y,chunkPos.z)), false);
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
WorldGenerator::generateTerrain(int ** map, int tx, int tz, int xSize, int ySize){
  //Generate base
  int px, nx;
  bool appendPX = getBorderHeight(tx + xSize, tz, intvec3(0,0,1), ySize, &px);
  bool appendNX = getBorderHeight(tx - 1, tz, intvec3(0,0,1), ySize, &nx);

  int pz, nz;
  bool appendPZ = getBorderHeight(tz + ySize, tz, intvec3(1,0,0), xSize, &pz);
  bool appendNZ = getBorderHeight(tz - 1, tz, intvec3(1,0,0), xSize, &nz);

  if(!appendPX && !appendNX){
     px = 0;
     nx = 0;
   } else if(!appendPX) {
     px = nx;
   } else if(!appendNX){
     nx = px;
   }

   if(!appendPZ && !appendNZ){
      pz = 0;
      nz = 0;
    } else if(!appendPZ) {
      pz = nz;
    } else if(!appendNZ){
      nz = pz;
    }

  for(int x = 0;x < xSize;x++){
    for(int y = 0;y < ySize;y++){
      int xh = nx+(px - nx)*x/xSize;
      int zh = nz+(pz - nz)*y/ySize;
      map[x][y] += (xh+zh)/2;
    }
  }

  //Generate peaks
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

bool WorldGenerator::getBorderHeight(int x, int z, intvec3 dir, int length, int * res){
  bool found = false;
  int sum = 0;
  int count = 0;
  for(int i =0;i<length;i++){
    int h;
    if(getHeightAt(x+dir.x*i, z+dir.z*i, &h)){
      sum+=h;
      count++;
      found = true;
    }
  }
  if(!found) return false;
  if(res != NULL) *res = sum/count;
  return true;
}

bool WorldGenerator::getHeightAt(int x, int z, int * res){
  for(int y = 30;y > -30;y--){
    if(isBlock(intvec3(x,y,z))){
      *res = y;
      return true;
    }
  }
  return false;
}

void WorldGenerator::placeTrees(int x, int z, int xSize, int zSize, int count){
  int * xPos = (int *) malloc(sizeof(int) * count);
  int * zPos = (int *) malloc(sizeof(int) * count);
  for(int i =0;i<count;i++){
    xPos[i] = x + rand()%xSize;
    zPos[i] = z + rand()%zSize;
  }
  for(int i =0;i<count;i++){
    int tx = xPos[i];
    int tz = zPos[i];
    int y;
    if(!getHeightAt(tx, tz, &y))continue;
    if(getBlock(intvec3(tx,y,tz))->type == Blocks::LEAVES) continue;
    y++;
    int h = rand()%7+2;
    for(int i =0;i<h;i++){
      intvec3 pos(tx,y+i,tz);
      Blocks::Block * bl = new Blocks::Wood();
      bl->pos = pos;
      addBlock(pos, bl, false);
    }
    intvec3 top(tx,y+h,tz);
    for(int dx = -3;dx < 3; dx++){
      for(int dy = -3;dy < 3; dy++){
        for(int dz = -3;dz < 3; dz++){
          if(dx ==0 && dz == 0 && dy<0) continue;
          if(sqrt(dx*dx+dy*dy+dz*dz) > 3.0) continue;
          intvec3 pos = top + intvec3(dx,dy,dz);
          Blocks::Block * bl = new Blocks::Leaves();
          bl->pos = pos;
          addBlock(pos, bl, false);
        }
      }
    }
  }
  free(xPos);
  free(zPos);
}
