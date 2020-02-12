#ifndef SAVE_MANAGER_HPP
#define SAVE_MANAGER_HPP

#include <glm/glm.hpp>
#include <sys/stat.h>
#include <string.h>
#include <mutex>
#include <fstream>

#include "zlib.h"

#include "inventory.hpp"
#include "blockArray.hpp"
#include "chunk.hpp"
#include "chunk.hpp"
#include "droppedBlock.hpp"
#include "blocks/blocks.hpp"



class Chunk;

class SaveManager{
public:
  static SaveManager * main;
  SaveManager(const char * savePath, GLuint mvpid);
  ~SaveManager();
  void saveHeader(BlockArray * chunks);
  void loadHeader();
  void saveChunk(Chunk * ch);
  void loadChunk(intvec3 pos, Chunk * ch);
  glm::vec3 loadPlayerPos();
  void savePlayerPos(glm::vec3 pos);
  glm::vec2 loadPlayerRot();
  void savePlayerRot(glm::vec2 rot);
  void saveInventory();
  void loadInventory();
  void saveDroppedBlocks();
  void loadDroppedBlocks();
  unsigned long loadSeed();
  void saveSeed(unsigned long seed);
  void compress();
  void cleanUp();
  size_t allocateBlockData(size_t size);
  FILE * getBlockDatafp();

  size_t dataFilePos;
  size_t blockDataFilePos;
private:
  void decompress();
  void write(BlockArray * arr, FILE * fp, int depth, size_t * currentDataPos, intvec3 pos);
  void filecat(FILE * fp, char * filename);
  size_t fileuncat(FILE * fp, char * filename);
  size_t fileuncat(FILE * fp, FILE * outfp);
  BlockArray * read(FILE * fp, int depth, intvec3 pos);

  GLuint mvpID;
  BlockArray * chunkPositions;
  char headerFilename[1024];
  char dataFilename[1024];
  char blockDataFilename[1024];
  char seedFilename[1024];
  char droppedBlocksFilename[1024];
  char targetFilename[1024];
  bool newFile = false;

  FILE * datafp;
  FILE * blockDatafp;
  std::mutex mtx;
};
#endif
