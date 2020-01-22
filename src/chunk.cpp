#include "chunk.hpp"


BlockArray * Chunk::chunks = new BlockArray();
GLuint Chunk::mvpID;
SaveManager * Chunk::saveManager;

Chunk *
Chunk::getChunk(intvec3 pos){
  BlockArray * yz = (BlockArray *)(chunks->get((long)pos.x));
  if(yz == NULL){
    chunks->set((long)pos.x,(void *)new BlockArray());
    yz = (BlockArray *)(chunks->get((long)pos.x));
  }
  BlockArray * z = (BlockArray *)(yz->get((long)pos.y));
  if(z == NULL){
    yz->set((long)pos.y,(void*)new BlockArray());
    z = (BlockArray *)(yz->get((long)pos.y));
  }
  Chunk * ch = (Chunk *)(z->get((long)pos.z));
  return ch;
}

void
Chunk::setChunk(intvec3 pos, Chunk * ch){
  BlockArray * yz = (BlockArray *)(chunks->get((long)pos.x));
  if(yz == NULL){
    chunks->set((long)pos.x,(void *)new BlockArray());
    yz = (BlockArray *)(chunks->get((long)pos.x));
  }
  BlockArray * z = (BlockArray *)(yz->get((long)pos.y));
  if(z == NULL){
    yz->set((long)pos.y,(void*)new BlockArray());
    z = (BlockArray *)(yz->get((long)pos.y));
  }
  z->set((long)pos.z,ch);
  saveManager->saveHeader(chunks);
}

Chunk::Chunk(intvec3 p){
  pos = p;
  modelMatrix = glm::translate(glm::mat4(1.0f),glm::vec3(pos.x*CHUNK_SIZE,pos.y*CHUNK_SIZE,pos.z*CHUNK_SIZE));
  for(int x =0;x<CHUNK_SIZE;x++){
    for(int y =0;y<CHUNK_SIZE;y++){
      for(int z =0;z<CHUNK_SIZE;z++){
        blocks[x][y][z] = false;
      }
    }
  }
  saveManager->loadChunk(pos,this);
  textureID = ResourceManager::getTexture("textures/grass.bmp");
}

void
Chunk::addBlock(intvec3 pos){
  blocks[pos.x][pos.y][pos.z] = true;
}

void
Chunk::removeBlock(intvec3 pos){
  blocks[pos.x][pos.y][pos.z] = false;
}

void
Chunk::update(bool save){
  recalculateSides();
  setGlBuffers();
  if(save) {
    saveManager->saveChunk(this);
  }
}
void
Chunk::recalculateSides(){
  sidesToRender.clear();
  int posXCount = 0;
  int negXCount = 0;
  int posYCount = 0;
  int negYCount = 0;
  int posZCount = 0;
  int negZCount = 0;
  for(int x =0;x<CHUNK_SIZE;x++){
    for(int y =0;y<CHUNK_SIZE;y++){
      for(int z =0;z<CHUNK_SIZE;z++){

        if(!blocks[x][y][z])continue;

        if(x == 0)negXCount++;
        if(x == CHUNK_SIZE -1) posXCount++;
        if(y == 0)negYCount++;
        if(y == CHUNK_SIZE -1)posYCount++;
        if(z == 0)negZCount++;
        if(z == CHUNK_SIZE -1)posZCount++;
        for(int d =-1;d<=1;d+=2){
          for(int r = 0;r<3;r++){
            int dx = r==0?d:0;
            int dy = r==1?d:0;
            int dz = r==2?d:0;
            intvec3 dir(dx,dy,dz);
            bool render = false;
            if(
              dx +x== -1||dx+x==CHUNK_SIZE||
              dy +y== -1||dy+y==CHUNK_SIZE||
              dz +z== -1||dz+z==CHUNK_SIZE)
              {
                render = true;
              } else {
                if(!blocks[dx+x][dy+y][dz+z]){
                  render = true;
                }
              }
              if(render){
                chunk_render_side side(intvec3(x,y,z),dir);
                sidesToRender.push_back(side);
              }
            }
          }
        }
      }
    }
    posX = (posXCount != CHUNK_SIZE*CHUNK_SIZE);
    negX = (negXCount != CHUNK_SIZE*CHUNK_SIZE);
    posY = (posYCount != CHUNK_SIZE*CHUNK_SIZE);
    negY = (negYCount != CHUNK_SIZE*CHUNK_SIZE);
    posZ = (posZCount != CHUNK_SIZE*CHUNK_SIZE);
    negZ = (negZCount != CHUNK_SIZE*CHUNK_SIZE);
    int doDrawConst = 1;
    for(int d =-1;d<=1;d+=2){
      for(int r = 0;r<3;r++){
        intvec3 dir(
          r==0?d:0,
          r==1?d:0,
          r==2?d:0
        );
        Chunk * ch = Chunk::getChunk(pos+dir);
        if(ch != NULL){
          bool render = canSeeThrough(dir);
          if(render) ch->doDraw |= doDrawConst; else ch->doDraw &= ~doDrawConst;
        }
        doDrawConst = doDrawConst << 1;
      }
    }
  }

bool
Chunk::canSeeThrough(intvec3 dir){
  if(dir == intvec3(1,0,0)) return posX;
  if(dir == intvec3(-1,0,0)) return negX;
  if(dir == intvec3(0,1,0)) return posY;
  if(dir == intvec3(0,-1,0)) return negY;
  if(dir == intvec3(0,0,1)) return posZ;
  if(dir == intvec3(0,0,-1)) return negZ;
}
void
Chunk::setGlBuffers(){
  std::vector<glm::vec3> vertices;
  vertices.reserve(sidesToRender.size()*6);
  std::vector<glm::vec2> UVs;
  UVs.reserve(sidesToRender.size()*6);
  for(int i =0;i<sidesToRender.size();i++){
    chunk_render_side side = sidesToRender[i];
    glm::vec3 pos(
      side.pos.x,
      side.pos.y,
      side.pos.z
    );
    if(side.dir.y == 1 || side.dir.y == -1){
      vertices[i*6 + 0] = pos+glm::vec3(-0.5f, side.dir.y*0.5f, 0.5f);
      vertices[i*6 + 1] = pos+glm::vec3(-0.5f, side.dir.y*0.5f,-0.5f);
      vertices[i*6 + 2] = pos+glm::vec3( 0.5f, side.dir.y*0.5f,-0.5f);

      vertices[i*6 + 3] = pos+glm::vec3(-0.5f, side.dir.y*0.5f, 0.5f);
      vertices[i*6 + 4] = pos+glm::vec3( 0.5f, side.dir.y*0.5f, 0.5f);
      vertices[i*6 + 5] = pos+glm::vec3( 0.5f, side.dir.y*0.5f,-0.5f);

      UVs[i*6 + 0] = glm::vec2( 0.0f, 1.0f);
      UVs[i*6 + 1] = glm::vec2( 0.0f, 0.0f);
      UVs[i*6 + 2] = glm::vec2( 1.0f, 0.0f);

      UVs[i*6 + 3] = glm::vec2( 0.0f, 1.0f);
      UVs[i*6 + 4] = glm::vec2( 1.0f, 1.0f);
      UVs[i*6 + 5] = glm::vec2( 1.0f, 0.0f);
    } else if(side.dir.x == 1 || side.dir.x == -1){
      vertices[i*6 + 0] = pos+glm::vec3(side.dir.x*0.5f,-0.5f, 0.5f);
      vertices[i*6 + 1] = pos+glm::vec3(side.dir.x*0.5f,-0.5f,-0.5f);
      vertices[i*6 + 2] = pos+glm::vec3(side.dir.x*0.5f, 0.5f,-0.5f);

      vertices[i*6 + 3] = pos+glm::vec3(side.dir.x*0.5f,-0.5f, 0.5f);
      vertices[i*6 + 4] = pos+glm::vec3(side.dir.x*0.5f, 0.5f, 0.5f);
      vertices[i*6 + 5] = pos+glm::vec3(side.dir.x*0.5f, 0.5f,-0.5f);

      UVs[i*6 + 0] = glm::vec2( 0.0f, 1.0f);
      UVs[i*6 + 1] = glm::vec2( 0.0f, 0.0f);
      UVs[i*6 + 2] = glm::vec2( 1.0f, 0.0f);

      UVs[i*6 + 3] = glm::vec2( 0.0f, 1.0f);
      UVs[i*6 + 4] = glm::vec2( 1.0f, 1.0f);
      UVs[i*6 + 5] = glm::vec2( 1.0f, 0.0f);
    } else if(side.dir.z == 1 || side.dir.z == -1){
      vertices[i*6 + 0] = pos+glm::vec3(-0.5f, 0.5f,side.dir.z*0.5f);
      vertices[i*6 + 1] = pos+glm::vec3(-0.5f,-0.5f,side.dir.z*0.5f);
      vertices[i*6 + 2] = pos+glm::vec3( 0.5f,-0.5f,side.dir.z*0.5f);

      vertices[i*6 + 3] = pos+glm::vec3(-0.5f, 0.5f,side.dir.z*0.5f);
      vertices[i*6 + 4] = pos+glm::vec3( 0.5f, 0.5f,side.dir.z*0.5f);
      vertices[i*6 + 5] = pos+glm::vec3( 0.5f,-0.5f,side.dir.z*0.5f);

      UVs[i*6 + 0] = glm::vec2( 0.0f, 1.0f);
      UVs[i*6 + 1] = glm::vec2( 0.0f, 0.0f);
      UVs[i*6 + 2] = glm::vec2( 1.0f, 0.0f);

      UVs[i*6 + 3] = glm::vec2( 0.0f, 1.0f);
      UVs[i*6 + 4] = glm::vec2( 1.0f, 1.0f);
      UVs[i*6 + 5] = glm::vec2( 1.0f, 0.0f);
    }
  }
  for(int i = 0;i<sidesToRender.size()*6;i++){
    glm::vec3 vertex = vertices[i];
    glm::vec2 uv = UVs[i];
  }
  if(vertexBuffer != -1) glDeleteBuffers(1,&vertexBuffer);
  if(uvBuffer != -1) glDeleteBuffers(1,&uvBuffer);
  glGenBuffers(1,&vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sidesToRender.size()*6*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1,&uvBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
  glBufferData(GL_ARRAY_BUFFER, sidesToRender.size()*6*sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);
}

void
Chunk::draw(glm::mat4 projection, glm::mat4 view){
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
  glDrawArrays(GL_TRIANGLES, 0, sidesToRender.size()*6);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}
