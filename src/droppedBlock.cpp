#include "droppedBlock.hpp"

std::vector<DroppedBlock *> DroppedBlock::droppedBlocks;

DroppedBlock::DroppedBlock(GLuint mvpid, Blocks::Block * bl, glm::vec3 pos){
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
    0.15f,-0.15f,-0.15f,

    //FLAT
    0.0f,-0.15f, 0.15f,
    0.0f,-0.15f,-0.15f,
    0.0f, 0.15f,-0.15f,
    0.0f,-0.15f, 0.15f,
    0.0f, 0.15f, 0.15f,
    0.0f, 0.15f,-0.15f,
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
  block = bl;
  textureID = Blocks::Block::getTextureFor(block->getType());
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
  float distance = glm::length(glm::vec3(modelMatrix[3])-playerPos);
  if(glm::length(velocity) < 0.5f) canPick = true;
  if(distance < 1.75f && Inventory::isPlaceFor(block->getBlockData())){
    if(canPick){
      velocity = (playerPos - glm::vec3(modelMatrix[3]))*5.0f;
      if(distance < 0.5f){
        Inventory::add(block);
        delete this;
        return;
      }
    }
  } else {
    velocity.y -= deltaTime*9.81f;
    canPick = true;
  }
  glm::vec3 pos = modelMatrix[3];
  intvec3 blockPos(
    round(pos.x),
    round(pos.y-0.15f),
    round(pos.z)
  );
  glm::vec3 vel = glm::vec3(0,0,0);
  float c = 0;
  for(float x = -1.0f;x<=1.0f;x+=2.0f){
    for(float y = -1.0f;y<=1.0f;y+=2.0f){
    for(float z = -1.0f;z<=1.0f;z+=2.0f){
      intvec3 p(
        round(pos.x+0.15f*x),
        round(pos.y+0.15f*y),
        round(pos.z+0.15f*z)
      );

      if(isBlock(p)){
        vel += glm::vec3(-x,-y,-z) * 0.1f;
        c++;
      }
    }
  }
  }
  if(c != 0){
    vel/=c;
    velocity = vel;
  }
  modelMatrix = glm::mat4(1.0f);
  modelMatrix[3] = glm::vec4(pos, 1.0f);
  modelMatrix = glm::translate(modelMatrix, deltaTime * velocity);
  modelMatrix = glm::rotate(modelMatrix, rotation, glm::vec3(0,1,0));
  rotation += deltaTime;
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
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
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
  if(block->drawFlat){
    glDrawArrays(GL_TRIANGLES, 36, 6);
  } else {
    glDrawArrays(GL_TRIANGLES, 0, 36);
  }

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}
