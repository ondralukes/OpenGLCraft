#include "saveManager.hpp"

SaveManager * SaveManager::main;
SaveManager::SaveManager(const char * savePath, GLuint mvpid){
  //Leave space for player position and rotation and inventory
  dataFilePos = sizeof(glm::vec3) + sizeof(glm::vec2) + sizeof(inventory_item) * 8;

  strcpy(headerFilename,savePath);
  strcat(headerFilename,".header");
  strcpy(dataFilename,savePath);
  strcat(dataFilename,".data");
  strcpy(blockDataFilename,savePath);
  strcat(blockDataFilename,".blockdata");
  strcpy(seedFilename,savePath);
  strcat(seedFilename,".seed");
  chunkPositions = new BlockArray();

  datafp = fopen(dataFilename,"wb+");
  blockDatafp = fopen(blockDataFilename,"wb+");

  char compressedDataFile[1030];
  sprintf(compressedDataFile,"%s.gz", dataFilename);
  char compressedHeaderFile[1030];
  sprintf(compressedHeaderFile,"%s.gz", headerFilename);
  char compressedBlockDataFile[1030];
  sprintf(compressedBlockDataFile,"%s.gz", blockDataFilename);
  if(std::ifstream(compressedDataFile).good() &&
    std::ifstream(compressedHeaderFile).good() &&
    std::ifstream(compressedBlockDataFile).good())
  {
    newFile = false;
    decompress();
  } else {
    newFile = true;
  }

  mvpID = mvpid;
}

SaveManager::~SaveManager(){
  delete chunkPositions;
}

void
SaveManager::saveHeader(BlockArray * chunks){
  const std::lock_guard<std::mutex> lock(mtx);
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
      pos.z = i;
      Chunk * ch = (Chunk *) arr->get(i);
      if(ch!=NULL){
        if(ch->posInFile ==0){
          ch->posInFile = *currentDataPos;
          *currentDataPos += sizeof(Blocks::block_data) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
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
          *currentDataPos += sizeof(Blocks::block_data) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
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
      if(posInFile + sizeof(Blocks::block_data) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE > dataFilePos){
        dataFilePos = posInFile + sizeof(Blocks::block_data) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
      }
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
      if(posInFile + sizeof(Blocks::block_data) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE > dataFilePos){
        dataFilePos = posInFile + sizeof(Blocks::block_data) * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
      }
    }
  }
  return arr;
}

void
SaveManager::loadHeader(){
  const std::lock_guard<std::mutex> lock(mtx);
  FILE * fp = fopen(headerFilename,"rb");
  if(fp==NULL) return;
  chunkPositions = read(fp,2,intvec3(0,0,0));
  fclose(fp);
}

void
SaveManager::saveChunk(Chunk * chunk){
  const std::lock_guard<std::mutex> lock(mtx);
  fseek(datafp, (long)chunk->posInFile, SEEK_SET);
  Blocks::block_data blockBuffer[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];
  int x = 0,y = 0,z=0;
  for(int i =0;i<CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE;i++){
    if(chunk->blocks[x][y][z] != NULL){
      blockBuffer[i] = chunk->blocks[x][y][z]->getBlockData();
    } else {
      Blocks::block_data data;
      data.type = Blocks::NONE;
      blockBuffer[i] = data;
    }
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
  fwrite(blockBuffer,sizeof(Blocks::block_data),CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE,datafp);
}

void
SaveManager::loadChunk(intvec3 pos, Chunk * ch){
  const std::lock_guard<std::mutex> lock(mtx);
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
    Blocks::block_data buffer[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];
    fread(buffer,sizeof(Blocks::block_data),CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE,datafp);
    int x = 0,y = 0,z=0;
    for(int i =0;i<CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE;i++){
      intvec3 blockPos(
        ch->pos.x*CHUNK_SIZE+x,
        ch->pos.y*CHUNK_SIZE+y,
        ch->pos.z*CHUNK_SIZE+z
      );
      ch->blocks[x][y][z] = Blocks::Block::decodeBlock(buffer[i], blockPos, mvpID);
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
    ch->update(false);
    ch->isLoaded = true;
  }
}

void
SaveManager::savePlayerPos(glm::vec3 pos){
  const std::lock_guard<std::mutex> lock(mtx);
  fseek(datafp,0,SEEK_SET);
  fwrite(&pos, sizeof(glm::vec3), 1, datafp);
}

glm::vec3
SaveManager::loadPlayerPos(){
  const std::lock_guard<std::mutex> lock(mtx);
  if(newFile) return glm::vec3(0.0f,100.0f,0.0f);
  glm::vec3 pos;
  fseek(datafp,0,SEEK_SET);
  fread(&pos, sizeof(glm::vec3), 1, datafp);
  return pos;
}

void
SaveManager::savePlayerRot(glm::vec2 rot){
  const std::lock_guard<std::mutex> lock(mtx);
  fseek(datafp, sizeof(glm::vec3), SEEK_SET);
  fwrite(&rot, sizeof(glm::vec2), 1, datafp);
}

glm::vec2
SaveManager::loadPlayerRot(){
  const std::lock_guard<std::mutex> lock(mtx);
  if(newFile) return glm::vec2(0.0f,0.0f);
  glm::vec2 rot;
  fseek(datafp, sizeof(glm::vec3), SEEK_SET);
  fread(&rot, sizeof(glm::vec2), 1, datafp);
  return rot;
}

void
SaveManager::compress(){
  const std::lock_guard<std::mutex> lock(mtx);
  char * buffer = (char *) malloc(sizeof(char)*1024*1024);

  char compressedDataFile[1030];
  sprintf(compressedDataFile,"%s.gz", dataFilename);
  gzFile gzfp = gzopen(compressedDataFile,"wb");
  fseek(datafp,0,SEEK_SET);
  while(!feof(datafp)){
    size_t dataRead = fread(buffer,sizeof(char),1024*1024, datafp);
    gzwrite(gzfp, buffer, dataRead);
  }
  gzclose(gzfp);

  char compressedBlockDataFile[1030];
  sprintf(compressedBlockDataFile,"%s.gz", blockDataFilename);
  gzfp = gzopen(compressedBlockDataFile,"wb");
  fseek(blockDatafp,0,SEEK_SET);
  while(!feof(blockDatafp)){
    size_t dataRead = fread(buffer,sizeof(char),1024*1024, blockDatafp);
    gzwrite(gzfp, buffer, dataRead);
  }
  gzclose(gzfp);

  char compressedHeaderFile[1030];
  sprintf(compressedHeaderFile,"%s.gz", headerFilename);
  gzfp = gzopen(compressedHeaderFile,"wb");
  fseek(datafp,0,SEEK_SET);
  FILE * fp = fopen(headerFilename,"rb");
  while(!feof(fp)){
    size_t dataRead = fread(buffer,sizeof(char),1024*1024, fp);
    gzwrite(gzfp, buffer, dataRead);
  }
  fclose(fp);
  gzclose(gzfp);
  free(buffer);
}

void
SaveManager::decompress(){
  const std::lock_guard<std::mutex> lock(mtx);
  char * buffer = (char *) malloc(1024*1024);

  char compressedDataFile[1030];
  sprintf(compressedDataFile,"%s.gz", dataFilename);
  gzFile gzfp = gzopen(compressedDataFile,"rb");
  fseek(datafp,0,SEEK_SET);
  while(!gzeof(gzfp)){
    size_t dataRead = gzread(gzfp, buffer,sizeof(char)*1024*1024);
    fwrite(buffer, sizeof(char), dataRead, datafp);
  }
  gzclose(gzfp);

  char compressedBlockDataFile[1030];
  sprintf(compressedBlockDataFile,"%s.gz", blockDataFilename);
  gzfp = gzopen(compressedBlockDataFile,"rb");
  fseek(blockDatafp,0,SEEK_SET);
  size_t sz = 0;
  while(!gzeof(gzfp)){
    size_t dataRead = gzread(gzfp, buffer,sizeof(char)*1024*1024);
    sz += dataRead;
    fwrite(buffer, sizeof(char), dataRead, blockDatafp);
  }
  blockDataFilePos = sz;
  gzclose(gzfp);

  char compressedHeaderFile[1030];
  sprintf(compressedHeaderFile,"%s.gz", headerFilename);
  gzfp = gzopen(compressedHeaderFile,"rb");
  fseek(datafp,0,SEEK_SET);
  FILE * fp = fopen(headerFilename,"wb");
  while(!gzeof(gzfp)){
    size_t dataRead = gzread(gzfp, buffer,sizeof(char)*1024*1024);
    fwrite(buffer, sizeof(char), dataRead, fp);
  }
  fclose(fp);
  gzclose(gzfp);
  free(buffer);
}

void
SaveManager::cleanUp(){
  fclose(datafp);
  fclose(blockDatafp);
  remove(dataFilename);
  remove(headerFilename);
  remove(blockDataFilename);
}

void
SaveManager::saveInventory(){
  const std::lock_guard<std::mutex> lock(mtx);
  fseek(datafp, sizeof(glm::vec3) + sizeof(glm::vec2), SEEK_SET);
  for(int i =0;i<8;i++){
    Blocks::block_data data;
    if(Inventory::inventory[i].block != NULL){
      data = Inventory::inventory[i].block->getBlockData();
    } else {
      data.type = Blocks::NONE;
      data.dataPos = 0;
    }
    fwrite(&data, sizeof(Blocks::block_data), 1, datafp);
    fwrite(&Inventory::inventory[i].count, sizeof(int), 1, datafp);
  }
}

void
SaveManager::loadInventory(){
  const std::lock_guard<std::mutex> lock(mtx);
  if(newFile) return;
  fseek(datafp, sizeof(glm::vec3) + sizeof(glm::vec2), SEEK_SET);
  for(int i =0;i<8;i++){
    Blocks::block_data data;
    fread(&data, sizeof(Blocks::block_data), 1, datafp);
    Inventory::inventory[i].block = Blocks::Block::decodeBlock(data, intvec3(0,0,0), 0);
    if(Inventory::inventory[i].block != NULL) Inventory::inventory[i].block->doDrop = false;
    fread(&Inventory::inventory[i].count, sizeof(int), 1, datafp);
  }
}

void
SaveManager::saveSeed(unsigned long seed){
  FILE * fp = fopen(seedFilename, "wb");
  fwrite(&seed, 1, sizeof(unsigned long), fp);
  fclose(fp);
}

unsigned long
SaveManager::loadSeed(){
  if(newFile){
    return rand()*rand()*rand();
  } else {
    unsigned long seed;
    FILE * fp = fopen(seedFilename, "rb");
    fread(&seed, 1, sizeof(unsigned long), fp);
    fclose(fp);
    return seed;
  }
}

size_t
SaveManager::allocateBlockData(size_t size){
  if(blockDataFilePos == 0) blockDataFilePos++;
  size_t pos = blockDataFilePos;
  fseek(blockDatafp, blockDataFilePos, SEEK_SET);
  fwrite("b", sizeof(char), size, blockDatafp);
  blockDataFilePos += size;
  return pos;
}

FILE *
SaveManager::getBlockDatafp(){
  return blockDatafp;
}
