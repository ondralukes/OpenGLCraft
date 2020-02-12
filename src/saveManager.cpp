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
  strcpy(droppedBlocksFilename,savePath);
  strcat(droppedBlocksFilename,".dropped");
  strcpy(targetFilename,savePath);
  strcat(targetFilename,".oglc");

  chunkPositions = new BlockArray();

  datafp = fopen(dataFilename,"wb+");
  blockDatafp = fopen(blockDataFilename,"wb+");

  if(std::ifstream(targetFilename).good())
  {
    newFile = false;
    decompress();
  } else {
    blockDataFilePos = 0;
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
  fclose(datafp);
  fclose(blockDatafp);
  FILE * fp = fopen("tmp", "wb");
  filecat(fp, dataFilename);
  filecat(fp, blockDataFilename);
  filecat(fp, headerFilename);
  filecat(fp, droppedBlocksFilename);
  filecat(fp, seedFilename);
  fclose(fp);

  char * buffer = (char *) malloc(1024*1024);

  fp = fopen("tmp", "rb");
  gzFile gzfp = gzopen(targetFilename,"wb");
  while(!feof(fp)){
    size_t dataRead = fread(buffer, 1, sizeof(char)*1024*1024, fp);
    gzwrite(gzfp, buffer, dataRead);
  }
  gzclose(gzfp);
  fclose(fp);
  remove("tmp");
  free(buffer);
}

void
SaveManager::decompress(){
  const std::lock_guard<std::mutex> lock(mtx);
  char * buffer = (char *) malloc(1024*1024);

  FILE * fp = fopen("tmp", "wb");
  gzFile gzfp = gzopen(targetFilename,"rb");
  while(!gzeof(gzfp)){
    size_t dataRead = gzread(gzfp, buffer,sizeof(char)*1024*1024);
    fwrite(buffer, sizeof(char), dataRead, fp);
  }
  gzclose(gzfp);
  fclose(fp);

  fp = fopen("tmp", "rb");
  fileuncat(fp, datafp);
  blockDataFilePos = fileuncat(fp, blockDatafp);
  fileuncat(fp, headerFilename);
  fileuncat(fp, droppedBlocksFilename);
  fileuncat(fp, seedFilename);
  fclose(fp);
  remove("tmp");
  free(buffer);
}

void
SaveManager::cleanUp(){
  remove(dataFilename);
  remove(headerFilename);
  remove(blockDataFilename);
  remove(seedFilename);
  remove(droppedBlocksFilename);
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

void
SaveManager::saveDroppedBlocks(){
  FILE * fp = fopen(droppedBlocksFilename, "wb");
  size_t count = DroppedBlock::droppedBlocks.size();
  fwrite(&count, 1, sizeof(size_t), fp);
  size_t nulls = 0;
  for(int i =0;i < count;i++){
    DroppedBlock * bl = DroppedBlock::droppedBlocks[i];
    if(bl == NULL){
      nulls++;
      continue;
    }
    Blocks::block_data data = bl->getBlock()->getBlockData();
    glm::vec3 p = bl->getPosition();
    fwrite(&p, 1, sizeof(glm::vec3), fp);
    fwrite(&data, 1, sizeof(Blocks::block_data), fp);
  }
  count -= nulls;
  fseek(fp, 0, SEEK_SET);
  fwrite(&count, 1, sizeof(size_t), fp);
  fclose(fp);
}

void
SaveManager::loadDroppedBlocks(){
  if(newFile) return;
  FILE * fp = fopen(droppedBlocksFilename, "rb");
  size_t count;
  fread(&count, 1, sizeof(size_t), fp);
  for(int i =0;i < count;i++){
    Blocks::block_data data;
    glm::vec3 p;
    fread(&p, 1, sizeof(glm::vec3), fp);
    fread(&data, 1, sizeof(Blocks::block_data), fp);
    DroppedBlock * bl = new DroppedBlock(
      GUI::mvpID,
      Blocks::Block::decodeBlock(
        data,
        intvec3(0,0,0),
        GUI::mvpID
      ),
      p
    );
  }
  fclose(fp);
}

void
SaveManager::filecat(FILE * fp, char * filename){
  struct stat s;
  stat(filename, &s);
  size_t size = s.st_size;
  fwrite(&size, 1, sizeof(size_t), fp);
  char * buffer = (char *) malloc(sizeof(char) * 1024 * 1024);
  FILE * infp = fopen(filename, "rb");
  while(!feof(infp)){
    size_t read = fread(buffer, 1, sizeof(char)*1024*1024, infp);
    fwrite(buffer, 1, read, fp);
  }
  fclose(infp);
  free(buffer);
}

size_t
SaveManager::fileuncat(FILE * fp, char * filename){
  size_t sz;
  size_t res;
  fread(&sz, 1, sizeof(size_t), fp);
  res=sz;
  char * buffer = (char *) malloc(sizeof(char) * 1024 * 1024);
  FILE * outfp = fopen(filename, "wb");
  while(sz > 0){
    size_t read = sizeof(char)*1024*1024;
    if(read > sz) read = sz;
    fread(buffer, 1, read, fp);
    fwrite(buffer, 1, read, outfp);
    if(read >= sz) break;
    sz -= read;
    if(feof(fp)){
      printf("Unexpected end-of-file\n");
      while(true);
    }
  }
  fclose(outfp);
  free(buffer);
  return res;
}

size_t
SaveManager::fileuncat(FILE * fp, FILE * outfp){
  size_t sz;
  size_t res;
  fread(&sz, 1, sizeof(size_t), fp);
  res=sz;
  char * buffer = (char *) malloc(sizeof(char) * 1024 * 1024);
  while(sz > 0){
    size_t read = sizeof(char)*1024*1024;
    if(read > sz) read = sz;
    fread(buffer, 1, read, fp);
    fwrite(buffer, 1, read, outfp);
    if(read >= sz) break;
    sz -= read;
    if(feof(fp)){
      printf("Unexpected end-of-file\n");
      while(true);
    }
  }
  free(buffer);
  return res;
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
