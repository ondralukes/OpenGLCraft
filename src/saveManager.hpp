#ifndef SAVE_MANAGER_HPP
#define SAVE_MANAGER_HPP

#include <glm/glm.hpp>
#include "blockArray.hpp"
#include "chunk.hpp"
#include <string.h>
#include <fstream>
#include "chunk.hpp"
#include "blocks/blocks.hpp"
#include "zlib.h"

class Chunk;

class SaveManager{
public:
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
  void compress();
  void cleanUp();
private:
  void decompress();
  void write(BlockArray * arr, FILE * fp, int depth, size_t * currentDataPos, intvec3 pos);
  BlockArray * read(FILE * fp, int depth, intvec3 pos);

  GLuint mvpID;
  BlockArray * chunkPositions;
  char headerFilename[1024];
  char dataFilename[1024];
  bool newFile = false;

  //Leave space for player position and rotation
  size_t dataFilePos = sizeof(glm::vec3) + sizeof(glm::vec2);
  FILE * datafp;
};
#endif
