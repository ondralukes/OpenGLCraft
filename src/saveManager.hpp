#ifndef SAVE_MANAGER_HPP
#define SAVE_MANAGER_HPP

#include "blockArray.hpp"
#include "chunk.hpp"

class Chunk;

class SaveManager{
public:
  SaveManager(const char * savePath);
  ~SaveManager();
  void saveHeader(BlockArray * chunks);
  void loadHeader();
  void saveChunk(Chunk * ch);
  void loadChunk(intvec3 pos, Chunk * ch);
private:
  void write(BlockArray * arr, FILE * fp, int depth, size_t * currentDataPos, intvec3 pos);
  BlockArray * read(FILE * fp, int depth, intvec3 pos);
  BlockArray * chunkPositions;
  char headerFilename[1024];
  char dataFilename[1024];
  size_t dataFilePos = 128;
  FILE * datafp;
};
#endif
