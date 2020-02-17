#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <atomic>
#include <memory>
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

#define CHUNK_SIZE 16

struct light_block{
  float value = 0.0f;
  bool isSource = false;
  intvec3 fromChunk = intvec3(0,-50,0);
};

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
  void destroyBlock(intvec3 pos);
  void draw(glm::mat4 projection, glm::mat4 view);
  void update(bool save = true);
  bool canSeeThrough(intvec3 dir);
  void recalculate();
  void updateLight(bool rec = true);
  int flowLight();
  void initLight();
  light_block getLight(intvec3 p, intvec3 * fromChunk = NULL);
  bool isEmpty = true;
  bool isLoaded = false;
  bool relight = true;
  bool noLight = true;
  int useFastLight = 2;
  int lightInitToken = -1;
  std::atomic<bool> isSafe;
  bool wasRecalculated = true;
  bool shouldRecalculate = true;
  static void updateSunlight(intvec3 p);
  Blocks::Block * blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
  light_block light[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

  //Used only in one chunk per terrain part
  bool terrainGenerated = false;
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
  GLuint lightBuffer = -1;
  std::mutex mtx;
  static const glm::vec2 upUVs[6];
  static const glm::vec2 downUVs[6];
  static const glm::vec2 frontUVs[6];
  static const glm::vec2 backUVs[6];
  static const glm::vec2 leftUVs[6];
  static const glm::vec2 rightUVs[6];
  static constexpr float margin = 0.005f;

  bool posX = true;
  bool negX = true;
  bool posY = true;
  bool negY = true;
  bool posZ = true;
  bool negZ = true;
};
#endif
