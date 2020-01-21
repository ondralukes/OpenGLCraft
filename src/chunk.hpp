#include <vector>
#include <cstdio>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "resourceManager.hpp"
#include "blockArray.hpp"
#include "structs.hpp"

#define CHUNK_SIZE 8

class Chunk{
public:
  static Chunk * getChunk(intvec3 pos);
  static void setChunk(intvec3 pos, Chunk * ch);

  Chunk(intvec3 pos);
  void addBlock(intvec3 pos);
  void removeBlock(intvec3 pos);
  void draw(glm::mat4 projection, glm::mat4 view);
  void update();
  bool canSeeThrough(intvec3 dir);
  bool blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
  static GLuint mvpID;
private:
  static BlockArray * chunks;
  void recalculateSides();
  void setGlBuffers();
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
