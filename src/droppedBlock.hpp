#ifndef DROPPED_BLOCK_HPP
#define DROPPED_BLOCK_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <vector>

#include "structs.hpp"
#include "blocks/block.hpp"
#include "blockUtils.hpp"
#include "inventory.hpp"

class DroppedBlock {
public:
  DroppedBlock(GLuint mvpid, Blocks::Block * bl, glm::vec3 pos);
  ~DroppedBlock();
  void setVelocity(glm::vec3 v);
  static void drawAll(glm::mat4 projection, glm::mat4 view);
  static void updateAll(float deltaTime, glm::vec3 playerPos);
  bool canPick = true;
private:
  static std::vector<DroppedBlock *> droppedBlocks;
  int id = 66666;
  void draw(glm::mat4 projection, glm::mat4 view);
  void update(float deltaTime, glm::vec3 playerPos);
  GLuint mvpID;
  GLuint textureID;
  Blocks::Block * block;
  GLuint vertexBuffer;
  GLuint uvBuffer;
  glm::vec3 velocity;
  glm::mat4 modelMatrix;
  float rotation = 0;
};
#endif
