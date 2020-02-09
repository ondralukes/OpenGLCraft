#include "worldGenerator.hpp"

#include "chunk.hpp"
#include "structs.hpp"
#include "blockUtils.hpp"
#include "blocks/blocks.hpp"

void
WorldGenerator::generate(glm::vec3 pos, float deltaTime){
  for(int tx = -3;tx<=3;tx++){
    for(int tz = -3;tz<=3;tz++){
      intvec3 chunkPos(
        floor(pos.x/CHUNK_SIZE)+tx,
        -1,
        floor(pos.z/CHUNK_SIZE)+tz
      );
      Chunk * ch = Chunk::getChunk(chunkPos);
      if(ch!=NULL){
        if(ch->terrainGenerated) continue;
      }
      if(ch == NULL){
        Chunk::setChunk(chunkPos,new Chunk(chunkPos), false);
        ch = Chunk::getChunk(chunkPos);
      }
      ch->terrainGenerated = true;
      if(!ch->isLoaded){
        int tchx = floor(chunkPos.x/(float)terrainChunkSize);
        int tchz = floor(chunkPos.z/(float)terrainChunkSize);

        std::vector<Peak *> peaks;
        for(int dtchx = -2;dtchx <= 2;dtchx++){
          for(int dtchz = -2;dtchz <= 2;dtchz++){
            getPeaks(tchx + dtchx, tchz + dtchz, &peaks);
          }
        }
        std::mt19937 rnd(seed * tchx*tchz + tchz);
        for(int dx = 0; dx < CHUNK_SIZE; dx++){
          for(int dz = 0; dz < CHUNK_SIZE; dz++){
            int x = chunkPos.x*CHUNK_SIZE + dx;
            int z = chunkPos.z*CHUNK_SIZE + dz;
            float val = 0.0f;
            for(int i = 0; i<peaks.size();i++){
              float v = peaks[i]->getValueAt(x ,z);
              val += v;
            }
            float dirtLayer = getRandFloat(&rnd) * 7.0f + 5.0f;
            for(int y = -7 * CHUNK_SIZE; y <= round(val); y++){
            intvec3 pos(
              x,
              y,
              z
            );
            Blocks::Block * bl;
            if(y == round(val)){
              bl = new Blocks::Grass();
            } else if(round(val) - y < dirtLayer){
              bl = new Blocks::Dirt();
            } else {
                bl = new Blocks::Stone();
            }
            bl->pos = pos;
            addBlock(pos, bl, false);
          }
          }
        }
        for(int i = 0; i<peaks.size();i++){
          delete peaks[i];
        }
        addTrees(chunkPos.x, chunkPos.z);
        Chunk::saveHeader();
        for(int y = -8;y<32;y++){
          ch = Chunk::getChunk(intvec3(chunkPos.x,y,chunkPos.z));
          if(ch==NULL){
            Chunk::setChunk(intvec3(chunkPos.x,y,chunkPos.z), new Chunk(intvec3(chunkPos.x,y,chunkPos.z)), false);
            ch = Chunk::getChunk(intvec3(chunkPos.x,y,chunkPos.z));
          }
          ch->update(true);
        }
      } else {
        for(int y = -8;y<32;y++){
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
}

void
WorldGenerator::addTrees(int chx, int chz){
  int tchx = floor(chx/(float)terrainChunkSize);
  int tchz = floor(chz/(float)terrainChunkSize);
  std::mt19937 rnd(seed * tchx + tchz + tchx*tchz*43);
  int count = floor(getRandFloat(&rnd) * 50.0f) + 30.0f;
  for(int i = 0;i < count; i++){
    int posX = floor(getRandFloat(&rnd) * terrainChunkSize * CHUNK_SIZE) - chx * CHUNK_SIZE + tchx * CHUNK_SIZE * terrainChunkSize;
    int posZ = floor(getRandFloat(&rnd) * terrainChunkSize * CHUNK_SIZE) - chz * CHUNK_SIZE + tchz * CHUNK_SIZE * terrainChunkSize;
    if(posX < 0 || posX >= CHUNK_SIZE) continue;
    if(posZ < 0 || posZ >= CHUNK_SIZE) continue;
    posX += chx * CHUNK_SIZE;
    posZ += chz * CHUNK_SIZE;
    int h = floor(getRandFloat(&rnd) * 3.0f) + 3;
    int y;
    if(!getHeightAt(posX, posZ, &y))continue;
    if(getBlock(intvec3(posX,y,posZ))->getType() == Blocks::LEAVES) continue;
    y++;
    for(int i =0;i<h;i++){
      intvec3 pos(posX,y+i,posZ);
      Blocks::Block * bl = new Blocks::Wood();
      bl->pos = pos;
      addBlock(pos, bl, false);
    }
    intvec3 top(posX,y+h,posZ);
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
}

bool
WorldGenerator::getHeightAt(int x, int z, int * res){
  for(int y = 32 * CHUNK_SIZE;y > -30;y--){
    if(isBlock(intvec3(x,y,z))){
      *res = y;
      return true;
    }
  }
  return false;
}

void
WorldGenerator::getPeaks(int tchx, int tchz, std::vector<Peak *> * peaks){
  std::mt19937 rnd(seed * tchx + tchz);
  int count = floor(getRandFloat(&rnd) * 20.0f);
  for(int i = 0;i < count; i++){
    int posX = floor(getRandFloat(&rnd) * terrainChunkSize * CHUNK_SIZE);
    int posZ = floor(getRandFloat(&rnd) * terrainChunkSize * CHUNK_SIZE);
    float a = 10.0f/(getRandFloat(&rnd) * 20.0f + 0.11f);
    float c = (getRandFloat(&rnd) * 22.0f + 4.0f);
    int absPosX = tchx * terrainChunkSize * CHUNK_SIZE + posX;
    int absPosZ = tchz * terrainChunkSize * CHUNK_SIZE + posZ;
    peaks->push_back(new Peak(absPosX, absPosZ, a, c));
  }
}

float
WorldGenerator::getRandFloat(std::mt19937 * rnd){
  return (float)((*rnd)() - rnd->min()) / (rnd->max() - rnd->min());
}

float
Peak::getValueAt(int _x, int _z){
  float d = sqrt(pow(_x-x, 2) + pow(_z-z, 2));
  float res = a*pow(2.718281828459045, -(d*d)/(2*c*c));
  return res;
}
