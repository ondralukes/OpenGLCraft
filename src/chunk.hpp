#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <map>
#include <cstdio>
#include <GL/glew.h>
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
  static Chunk * getChunk(intvec3 pos);
  static void setChunk(intvec3 pos, Chunk * ch);

  Chunk(intvec3 pos);
  void addBlock(intvec3 pos, Blocks::Block * bl);
  void removeBlock(intvec3 pos);
  void draw(glm::mat4 projection, glm::mat4 view);
  void update(bool save = true);
  bool canSeeThrough(intvec3 dir);
  void recalculateSides();
  bool isLoaded = false;
  Blocks::Block * blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
  size_t posInFile = 0;
  int doDraw = 0;
private:
  void setGlBuffers();
  intvec3 pos;
  std::vector<chunk_render_side> sidesToRender;
  glm::mat4 modelMatrix;
  GLuint vertexBuffer = -1;
  GLuint uvBuffer = -1;

  bool posX = true;
  bool negX = true;
  bool posY = true;
  bool negY = true;
  bool posZ = true;
  bool negZ = true;
};
#endif
