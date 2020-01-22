#include "saveManager.hpp"
#include <string.h>
#include <fstream>
#include "chunk.hpp"

SaveManager::SaveManager(const char * savePath){
  strcpy(headerFilename,savePath);
  strcat(headerFilename,".header");
  strcpy(dataFilename,savePath);
  strcat(dataFilename,".data");
  chunkPositions = new BlockArray();
  char tmpFile[1024];
  sprintf(tmpFile,"%stmp",dataFilename);
  std::ifstream  src(dataFilename, std::ios::binary);
  std::ofstream  dst(tmpFile, std::ios::binary);
  dst << src.rdbuf();

  FILE * oldFp = fopen(tmpFile,"rb");
  datafp = fopen(dataFilename,"wb+");
  char buffer[2048];
  while(!feof(oldFp)){
    size_t read = fread(buffer, sizeof(char),2048,oldFp);
    fwrite(buffer, sizeof(char),read,datafp);
  }
  fclose(oldFp);

}

SaveManager::~SaveManager(){
  fclose(datafp);
  delete chunkPositions;
}

void
SaveManager::saveHeader(BlockArray * chunks){
  FILE * fp = fopen(headerFilename,"wb");
  size_t currentDataPos = dataFilePos;
  write(chunks,fp,2,&currentDataPos,intvec3(0,0,0));
  dataFilePos = currentDataPos;
  fclose(fp);
}

void
SaveManager::write(BlockArray * arr, FILE * fp, int depth, size_t * currentDataPos, intvec3 pos){
  if(arr == NULL){
    size_t zero = 0;
    fwrite(&zero, sizeof(size_t), 1,fp);
    fwrite(&zero, sizeof(size_t), 1,fp);
    return;
  }
  fwrite(&(arr->positiveArrayLength), sizeof(size_t), 1,fp);
  for(size_t i = 0;i<arr->positiveArrayLength;i++){
    if(depth > 0){
      if(depth == 2){
        pos.x = i;
      } else {
        pos.y = i;
      }
      write((BlockArray *) arr->get(i),fp,depth-1, currentDataPos,pos);
    } else {
      pos.z = -i;
      Chunk * ch = (Chunk *) arr->get(i);
      if(ch!=NULL){
        if(ch->posInFile ==0){
          ch->posInFile = *currentDataPos;
          *currentDataPos += sizeof(char) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
        }
        fwrite(&(ch->posInFile),sizeof(size_t), 1, fp);

      } else {
        size_t zero = 0;
        fwrite(&zero,sizeof(size_t), 1, fp);
      }
    }
  }
  fwrite(&(arr->negativeArrayLength), sizeof(size_t), 1,fp);
  for(size_t i = 0;i<arr->negativeArrayLength;i++){
    if(depth > 0){
      if(depth == 2){
        pos.x = -i-1;
      } else {
        pos.y = -i-1;
      }
      write((BlockArray *)arr->get(-i-1),fp,depth-1,currentDataPos,pos);
    } else {
      pos.z = -i-1;
      Chunk * ch = (Chunk *) arr->get(-i-1);
      if(ch!=NULL){
        if(ch->posInFile == 0){
          ch->posInFile = *currentDataPos;
          *currentDataPos += sizeof(char) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
        }
        fwrite(&(ch->posInFile),sizeof(size_t), 1, fp);
      } else {
        size_t zero = 0;
        fwrite(&zero,sizeof(size_t), 1, fp);
      }
    }
  }
}

BlockArray *
SaveManager::read(FILE * fp, int depth, intvec3 pos){
  BlockArray * arr = new BlockArray();
  size_t positiveArrayLength = 0;
  fread(&positiveArrayLength, sizeof(size_t), 1,fp);
  for(size_t i = 0;i<positiveArrayLength;i++){
    if(depth > 0){
      if(depth == 2){
        pos.x = i;
      } else {
        pos.y = i;
      }
      arr->set(i,(void *)read(fp,depth-1,pos));
    } else {
      pos.z = i;
      size_t posInFile;
      fread(&posInFile,sizeof(size_t), 1, fp);
      arr->set(i,(void *) posInFile);
      dataFilePos = sizeof(char) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
    }
  }
  size_t negativeArrayLength = 0;
  fread(&negativeArrayLength, sizeof(size_t), 1,fp);
  for(size_t i = 0;i<negativeArrayLength;i++){
    if(depth > 0){
      if(depth == 2){
        pos.x = -i-1;
      } else {
        pos.y = -i-1;
      }
      arr->set(-i-1,read(fp,depth-1,pos));
    } else {
      pos.z = -i-1;
      size_t posInFile;
      fread(&posInFile,sizeof(size_t), 1, fp);
      arr->set(-i-1,(void *) posInFile);
      dataFilePos = sizeof(char) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
    }
  }
  return arr;
}

void
SaveManager::loadHeader(){
  FILE * fp = fopen(headerFilename,"rb");
  if(fp==NULL) return;
  chunkPositions = read(fp,2,intvec3(0,0,0));
  fclose(fp);
}

void
SaveManager::saveChunk(Chunk * chunk){
  fseek(datafp, (long)chunk->posInFile, SEEK_SET);
  bool blockBuffer[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];
  int x = 0,y = 0,z=0;
  for(int i =0;i<CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE;i++){
    blockBuffer[i] = chunk->blocks[x][y][z];
    x++;
    if(x==CHUNK_SIZE){
      x=0;
      y++;
      if(y==CHUNK_SIZE){
        y=0;
        z++;
      }
    }
  }
  fwrite(blockBuffer,sizeof(bool),CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE,datafp);
}

void
SaveManager::loadChunk(intvec3 pos, Chunk * ch){
  if(ch == NULL) printf("ERROR");
  BlockArray * yz = (BlockArray *)(chunkPositions->get((long)pos.x));
  if(yz == NULL){
    chunkPositions->set((long)pos.x,(void *)new BlockArray());
    yz = (BlockArray *)(chunkPositions->get((long)pos.x));
  }
  BlockArray * z = (BlockArray *)(yz->get((long)pos.y));
  if(z == NULL){
    yz->set((long)pos.y,(void*)new BlockArray());
    z = (BlockArray *)(yz->get((long)pos.y));
  }
  size_t posInFile = (size_t)(z->get((long)pos.z));
  if(posInFile != 0){
    ch->posInFile = posInFile;
    if(datafp == NULL) return;
    fseek(datafp, (long)ch->posInFile, SEEK_SET);
    bool buffer[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];
    fread(buffer,sizeof(bool),CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE,datafp);
    int x = 0,y = 0,z=0;
    for(int i =0;i<CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE;i++){
      ch->blocks[x][y][z] = buffer[i];
      x++;
      if(x==CHUNK_SIZE){
        x=0;
        y++;
        if(y==CHUNK_SIZE){
          y=0;
          z++;
        }
      }
    }
    ch->update();
    ch->isLoaded = true;
  }
}
