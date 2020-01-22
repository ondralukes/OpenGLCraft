#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstdio>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "resourceManager.hpp"
#include "blockArray.hpp"
#include "structs.hpp"
#include "saveManager.hpp"

class SaveManager;

#define CHUNK_SIZE 8

class Chunk{
public:
  static Chunk * getChunk(intvec3 pos);
  static void setChunk(intvec3 pos, Chunk * ch);

  Chunk(intvec3 pos);
  void addBlock(intvec3 pos);
  void removeBlock(intvec3 pos);
  void draw(glm::mat4 projection, glm::mat4 view);
  void update(bool save = true);
  bool canSeeThrough(intvec3 dir);
  bool isLoaded = false;
  bool blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
  size_t posInFile = 0;
  int doDraw = 0;
  static GLuint mvpID;
  static SaveManager * saveManager;
  static BlockArray * chunks;
private:
  void recalculateSides();
  void setGlBuffers();
  intvec3 pos;
  std::vector<chunk_render_side> sidesToRender;
  glm::mat4 modelMatrix;
  GLuint vertexBuffer = -1;
  GLuint uvBuffer = -1;
  GLuint textureID;

  bool posX = true;
  bool negX = true;
  bool posY = true;
  bool negY = true;
  bool posZ = true;
  bool negZ = true;
};
#endif
