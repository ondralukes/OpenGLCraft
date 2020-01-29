#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <map>
#include <mutex>
#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "resourceManager.hpp"
#include "blockArray.hpp"
#include "structs.hpp"
#include "saveManager.hpp"
#include "blocks/blocks.hpp"

class SaveManager;

#define CHUNK_SIZE 8

class Chunk{
public:
  static GLuint mvpID;
  static SaveManager * saveManager;
  static BlockArray * chunks;
  static void saveHeader();
  static Chunk * getChunk(intvec3 pos);
  static void setChunk(intvec3 pos, Chunk * ch, bool saveHeaders = true);
  Chunk(intvec3 pos);
  void addBlock(intvec3 pos, Blocks::Block * bl);
  void removeBlock(intvec3 pos);
  void draw(glm::mat4 projection, glm::mat4 view);
  void update(bool save = true);
  bool canSeeThrough(intvec3 dir);
  void recalculate();
  bool isLoaded = false;
  bool wasRecalculated = true;
  bool shouldRecalculate = true;
  Blocks::Block * blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
  size_t posInFile = 0;
  int doDraw = 0;
  intvec3 pos;
private:
  static std::mutex stmtx;
  void recalculateSides();
  void setGlBuffers();
  std::vector<chunk_render_side> sidesToRender;
  glm::mat4 modelMatrix;
  GLuint vertexBuffer = -1;
  GLuint uvBuffer = -1;
  std::mutex mtx;

  bool posX = true;
  bool negX = true;
  bool posY = true;
  bool negY = true;
  bool posZ = true;
  bool negZ = true;
};
#endif
