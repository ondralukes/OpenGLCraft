#include "droppedBlock.hpp"

std::vector<DroppedBlock *> DroppedBlock::droppedBlocks;

DroppedBlock::DroppedBlock(GLuint mvpid, GLuint texID, glm::vec3 pos){
  static const float vertices[]{
    //UP
    -0.15f, 0.15f, 0.15f,
    -0.15f, 0.15f,-0.15f,
    0.15f, 0.15f,-0.15f,
    -0.15f, 0.15f, 0.15f,
    0.15f, 0.15f, 0.15f,
    0.15f, 0.15f,-0.15f,
    //DOWN
    -0.15f, -0.15f, 0.15f,
    -0.15f, -0.15f,-0.15f,
    0.15f, -0.15f,-0.15f,
    -0.15f, -0.15f, 0.15f,
    0.15f, -0.15f, 0.15f,
    0.15f, -0.15f,-0.15f,
    //RIGHT
    0.15f,-0.15f, 0.15f,
    0.15f,-0.15f,-0.15f,
    0.15f, 0.15f,-0.15f,
    0.15f,-0.15f, 0.15f,
    0.15f, 0.15f, 0.15f,
    0.15f, 0.15f,-0.15f,
    //LEFT
    -0.15f,-0.15f, 0.15f,
    -0.15f,-0.15f,-0.15f,
    -0.15f, 0.15f,-0.15f,
    -0.15f,-0.15f, 0.15f,
    -0.15f, 0.15f, 0.15f,
    -0.15f, 0.15f,-0.15f,
    //FRONT
    -0.15f, 0.15f,0.15f,
    -0.15f,-0.15f,0.15f,
    0.15f,-0.15f,0.15f,
    -0.15f, 0.15f,0.15f,
    0.15f, 0.15f,0.15f,
    0.15f,-0.15f,0.15f,
    //BACK
    -0.15f, 0.15f,-0.15f,
    -0.15f,-0.15f,-0.15f,
    0.15f,-0.15f,-0.15f,
    -0.15f, 0.15f,-0.15f,
    0.15f, 0.15f,-0.15f,
    0.15f,-0.15f,-0.15f
  };
  static const float UVs[]{
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
  };
  glGenBuffers(1,&vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenBuffers(1,&uvBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(UVs), UVs, GL_STATIC_DRAW);

  modelMatrix = glm::translate(glm::mat4(1.0f),pos);
  mvpID = mvpid;
  textureID = texID;
  bool gotId = false;
  for(int i =0;i<droppedBlocks.size();i++){
    if(droppedBlocks[i] == NULL){
      id = i;
      gotId = true;
      droppedBlocks[id] = this;
      break;
    }
  }
  if(!gotId){
    id = droppedBlocks.size();
    droppedBlocks.push_back(this);
  }
}

DroppedBlock::~DroppedBlock(){
  glDeleteBuffers(1, &vertexBuffer);
  glDeleteBuffers(1, &uvBuffer);
  droppedBlocks[id] = NULL;
}

void
DroppedBlock::setVelocity(glm::vec3 v){
  velocity = v;
}
void
DroppedBlock::updateAll(float deltaTime, glm::vec3 playerPos){
  for(int i =0;i<droppedBlocks.size();i++){
    if(droppedBlocks[i] != NULL) droppedBlocks[i]->update(deltaTime, playerPos);
  }
}

void
DroppedBlock::update(float deltaTime, glm::vec3 playerPos){
  float distance = glm::length(glm::vec3(modelMatrix[3])-playerPos) ;
  if(distance < 2.0f){
    velocity += (playerPos - glm::vec3(modelMatrix[3]))*0.1f;
    if(distance < 0.5f){
      delete this;
      return;
    }
  }
  velocity.y -= deltaTime*9.81f;
  glm::vec3 pos = modelMatrix[3];
  intvec3 blockPos(
    round(pos.x),
    round(pos.y-0.15f),
    round(pos.z)
  );
  if(!isBlock(blockPos) || velocity.y > 0){
    modelMatrix = glm::translate(modelMatrix, deltaTime * velocity);
  } else {
    velocity = glm::vec3(0, 0, 0);
  }
}

void
DroppedBlock::drawAll(glm::mat4 projection, glm::mat4 view){
  for(int i =0;i<droppedBlocks.size();i++){
    if(droppedBlocks[i] != NULL) droppedBlocks[i]->draw(projection, view);
  }
}

void
DroppedBlock::draw(glm::mat4 projection, glm::mat4 view){
  glm::mat4 mvp = projection*view*modelMatrix;
  glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glUniform1i(textureID, 0);
  //vertex
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glVertexAttribPointer(
    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
    3,                  // size
    GL_FLOAT,           // type
    GL_FALSE,           // normalized?
    0,                  // stride
    (void*)0            // array buffer offset
  );

  //color
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
  glVertexAttribPointer(
    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
    2,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  glDrawArrays(GL_TRIANGLES, 0, 36);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}
