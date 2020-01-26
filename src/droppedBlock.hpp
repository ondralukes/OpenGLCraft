#ifndef DROPPED_BLOCK_HPP
#define DROPPED_BLOCK_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <vector>

#include "structs.hpp"
#include "blockUtils.hpp"

class DroppedBlock {
public:
  DroppedBlock(GLuint mvpid, GLuint texID, glm::vec3 pos);
  ~DroppedBlock();
  void setVelocity(glm::vec3 v);
  static void drawAll(glm::mat4 projection, glm::mat4 view);
  static void updateAll(float deltaTime, glm::vec3 playerPos);
private:
  static std::vector<DroppedBlock *> droppedBlocks;
  int id = 66666;
  void draw(glm::mat4 projection, glm::mat4 view);
  void update(float deltaTime, glm::vec3 playerPos);
  GLuint mvpID;
  GLuint textureID;
  GLuint vertexBuffer;
  GLuint uvBuffer;
  glm::vec3 velocity;
  glm::mat4 modelMatrix;
};
#endif