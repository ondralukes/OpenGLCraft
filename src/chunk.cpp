#include "chunk.hpp"


BlockArray * Chunk::chunks = new BlockArray();
GLuint Chunk::mvpID;
SaveManager * Chunk::saveManager;
std::mutex Chunk::stmtx;
std::vector<intvec3> Chunk::chunkPositions;
int Chunk::chunksLoaded = 0;

const glm::vec2 Chunk::upUVs[6] = {
  glm::vec2( 0.75f + margin, 0.25f - margin),
  glm::vec2( 0.75f + margin, 0.0f + margin),
  glm::vec2( 1.0f  - margin, 0.0f + margin),

  glm::vec2( 0.75f + margin, 0.25f - margin),
  glm::vec2( 1.0f - margin,  0.25f - margin),
  glm::vec2( 1.0f - margin,  0.0f + margin)
};

const glm::vec2 Chunk::downUVs[6] = {
  glm::vec2( 0.25f - margin, 0.75f - margin),
  glm::vec2( 0.25f - margin, 0.50f + margin),
  glm::vec2( 0.0f + margin, 0.50f + margin),

  glm::vec2( 0.25f - margin, 0.75f - margin),
  glm::vec2( 0.0f + margin,  0.75f - margin),
  glm::vec2( 0.0f + margin,  0.50f + margin)
};

const glm::vec2 Chunk::frontUVs[6] = {
  glm::vec2( 0.0f + margin, 0.25f + margin),
  glm::vec2( 0.0f + margin, 0.50f - margin),
  glm::vec2( 0.25f - margin, 0.50f - margin),

  glm::vec2( 0.0f + margin, 0.25f + margin),
  glm::vec2( 0.25f - margin,  0.25f + margin),
  glm::vec2( 0.25f - margin,  0.50f - margin)
};

const glm::vec2 Chunk::backUVs[6] = {
  glm::vec2( 0.75f - margin, 0.25f + margin),
  glm::vec2( 0.75f - margin, 0.50f - margin),
  glm::vec2( 0.50f + margin, 0.50f - margin),

  glm::vec2( 0.75f - margin, 0.25f + margin),
  glm::vec2( 0.50f + margin,  0.25f + margin),
  glm::vec2( 0.50f + margin,  0.50f - margin)
};

const glm::vec2 Chunk::leftUVs[6] = {
  glm::vec2( 1.0f - margin, 0.50f - margin),
  glm::vec2( 0.75f + margin, 0.50f - margin),
  glm::vec2( 0.75f + margin, 0.25f + margin),

  glm::vec2( 1.0f - margin, 0.50f - margin),
  glm::vec2( 1.0f - margin,  0.25f + margin),
  glm::vec2( 0.75f + margin,  0.25f + margin)
};

const glm::vec2 Chunk::rightUVs[6] = {
  glm::vec2( 0.50f - margin, 0.50f - margin),
  glm::vec2( 0.25f + margin, 0.50f - margin),
  glm::vec2( 0.25f + margin, 0.25f + margin),


  glm::vec2( 0.50f - margin, 0.50f - margin),
  glm::vec2( 0.50f - margin,  0.25f + margin),
  glm::vec2( 0.25f + margin,  0.25f + margin)
};

Chunk *
Chunk::getChunk(intvec3 pos){
  BlockArray * yz = (BlockArray *)(chunks->get((long)pos.x));
  if(yz == NULL){
    return NULL;
  }
  BlockArray * z = (BlockArray *)(yz->get((long)pos.y));
  if(z == NULL){
    return NULL;
  }
  Chunk * ch = (Chunk *)(z->get((long)pos.z));
  return ch;
}

void
Chunk::setChunk(intvec3 pos, Chunk * ch, bool saveHeaders){
  const std::lock_guard<std::mutex> lock(stmtx);
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
  if(saveHeaders) saveManager->saveHeader(chunks);
}

Chunk::Chunk(intvec3 p){
  isSafe = false;
  pos = p;
  modelMatrix = glm::translate(glm::mat4(1.0f),glm::vec3(pos.x*CHUNK_SIZE,pos.y*CHUNK_SIZE,pos.z*CHUNK_SIZE));
  for(int x =0;x<CHUNK_SIZE;x++){
    for(int y =0;y<CHUNK_SIZE;y++){
      for(int z =0;z<CHUNK_SIZE;z++){
        blocks[x][y][z] = NULL;
      }
    }
  }
  bool inserted = false;
  for(int i = 0;i<chunkPositions.size();i++){
    if(chunkPositions[i] == intvec3(0,666,0)){
      chunkPositions[i] = pos;
      inserted = true;
      break;
    }
  }
  if(!inserted) chunkPositions.push_back(pos);
  saveManager->loadChunk(pos,this);
  chunksLoaded++;
}

void
Chunk::deleteDistChunks(intvec3 pos){
  for(int i = 0;i<chunkPositions.size();i++){
    if(pos.x - chunkPositions[i].x<-3||pos.x - chunkPositions[i].x>3
    || pos.z - chunkPositions[i].z<-3||pos.z - chunkPositions[i].z>3){
      Chunk * ch = Chunk::getChunk(chunkPositions[i]);
      if(ch != NULL){
        delete ch;
        Chunk::setChunk(chunkPositions[i], NULL, false);
        chunkPositions[i] = intvec3(0,666,0);
      }
    }
  }
}

Chunk::~Chunk(){
  chunksLoaded--;
  destroying = true;
  isSafe = false;
  for(int x =0;x<CHUNK_SIZE;x++){
    for(int y =0;y<CHUNK_SIZE;y++){
      for(int z =0;z<CHUNK_SIZE;z++){
        Blocks::Block * tmp = blocks[x][y][z];
        if(tmp == NULL) continue;
        tmp->doDrop = false;
        delete tmp;
        blocks[x][y][z] = NULL;
      }
    }
  }
  if(vertexBuffer != -1){
  glDeleteBuffers(1, &vertexBuffer);
  glDeleteBuffers(1, &uvBuffer);
  glDeleteBuffers(1, &lightBuffer);
}
}

void
Chunk::saveHeader(){
  saveManager->saveHeader(Chunk::chunks);
}

void
Chunk::update(bool save){
  shouldRecalculate = true;
  if(save) {
    if(!this->save){
      //Save chunk in header
      saveManager->saveHeader(Chunk::chunks);
    }
    this->save = true;
    saveManager->saveChunk(this);
  }
}

void
Chunk::recalculate(){
  recalculateSides();

  //Recalculate adjacent chunks to update doDraw
  for(int d =-1;d<=1;d+=2){
    for(int r = 0;r<3;r++){
      intvec3 dir(
        r==0?d:0,
        r==1?d:0,
        r==2?d:0
      );
      Chunk * ch = Chunk::getChunk(pos+dir);
      if(ch != NULL){
        ch->recalculateSides();
        ch->wasRecalculated = true;
      }
    }
  }
  shouldRecalculate = false;
  wasRecalculated = true;
}

void
Chunk::recalculateSides(){
  if(destroying) return;
  const std::lock_guard<std::mutex> lock(mtx);
  sidesToRender.clear();
  bool empty = true;
  std::map<GLuint, std::vector<chunk_render_side>> textureRenderMap;
  int posXCount = 0;
  int negXCount = 0;
  int posYCount = 0;
  int negYCount = 0;
  int posZCount = 0;
  int negZCount = 0;
  for(int x =0;x<CHUNK_SIZE;x++){
    for(int y =0;y<CHUNK_SIZE;y++){
      for(int z =0;z<CHUNK_SIZE;z++){

        if(blocks[x][y][z]==NULL)continue;

        empty = false;

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
                if(blocks[dx+x][dy+y][dz+z]==NULL){
                  render = true;
                }
              }
              if(render){
                light_block lbl = getLight(intvec3(x, y, z) + dir);
                float l = 0.5f;
                if(lbl.ready){
                  l=lbl.value;
                } else {
                  for(int d =-1;d<=1;d+=2){
                    for(int r = 0;r<3;r++){
                      intvec3 rdydir(
                        r==0?d:0,
                        r==1?d:0,
                        r==2?d:0
                      );
                      light_block rdylbl =  getLight(intvec3(x, y, z) + dir + rdydir);
                      if(rdylbl.ready && rdylbl.value > l){
                        l = rdylbl.value;
                      }
                    }
                  }
                }
                Blocks::Block * bl = blocks[x][y][z];
                if(bl == NULL) continue;
                chunk_render_side side(intvec3(x,y,z), dir, bl->textureID, bl->damageLevel, l);
                int key = bl->damageLevel==0?bl->textureID +9:bl->damageLevel;
                if(textureRenderMap.find(key) == textureRenderMap.end()){
                  textureRenderMap.insert({key, std::vector<chunk_render_side>()});
                }
                textureRenderMap[key].push_back(side);
              }
            }
          }
        }
      }
    }

    //Put textures together
    std::map<GLuint, std::vector<chunk_render_side>>::iterator it;
    for(it = textureRenderMap.begin(); it != textureRenderMap.end(); it++){
      for(int i =0;i<it->second.size();i++){
        sidesToRender.push_back(it->second[i]);
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
    isEmpty = empty;
    isSafe = true;
  }

bool
Chunk::canSeeThrough(intvec3 dir){
  if(dir == intvec3(1,0,0)) return posX;
  if(dir == intvec3(-1,0,0)) return negX;
  if(dir == intvec3(0,1,0)) return posY;
  if(dir == intvec3(0,-1,0)) return negY;
  if(dir == intvec3(0,0,1)) return posZ;
  if(dir == intvec3(0,0,-1)) return negZ;
  return true;
}

void
Chunk::setGlBuffers(){
  std::vector<glm::vec3> vertices;
  vertices.resize(sidesToRender.size()*6);
  std::vector<glm::vec2> UVs;
  UVs.resize(sidesToRender.size()*6);
  std::vector<float> lights;
  lights.resize(sidesToRender.size()*6);
  for(int i =0;i<sidesToRender.size();i++){
    chunk_render_side side = sidesToRender[i];
    glm::vec3 pos(
      side.pos.x,
      side.pos.y,
      side.pos.z
    );

    for(int j = 0;j<6;j++){
      lights[i*6 + j] = side.light;
    }
    if(side.dir.y == 1 || side.dir.y == -1){
      vertices[i*6 + 0] = pos+glm::vec3(-0.5f, side.dir.y*0.5f, 0.5f);
      vertices[i*6 + 1] = pos+glm::vec3(-0.5f, side.dir.y*0.5f,-0.5f);
      vertices[i*6 + 2] = pos+glm::vec3( 0.5f, side.dir.y*0.5f,-0.5f);

      vertices[i*6 + 3] = pos+glm::vec3(-0.5f, side.dir.y*0.5f, 0.5f);
      vertices[i*6 + 4] = pos+glm::vec3( 0.5f, side.dir.y*0.5f, 0.5f);
      vertices[i*6 + 5] = pos+glm::vec3( 0.5f, side.dir.y*0.5f,-0.5f);

      if(side.dir.y == 1){
        memcpy(&UVs[i*6], upUVs, 6*sizeof(glm::vec2));
      } else {
        memcpy(&UVs[i*6], downUVs, 6*sizeof(glm::vec2));
      }
    } else if(side.dir.x == 1 || side.dir.x == -1){
      vertices[i*6 + 0] = pos+glm::vec3(side.dir.x*0.5f,-0.5f, 0.5f);
      vertices[i*6 + 1] = pos+glm::vec3(side.dir.x*0.5f,-0.5f,-0.5f);
      vertices[i*6 + 2] = pos+glm::vec3(side.dir.x*0.5f, 0.5f,-0.5f);

      vertices[i*6 + 3] = pos+glm::vec3(side.dir.x*0.5f,-0.5f, 0.5f);
      vertices[i*6 + 4] = pos+glm::vec3(side.dir.x*0.5f, 0.5f, 0.5f);
      vertices[i*6 + 5] = pos+glm::vec3(side.dir.x*0.5f, 0.5f,-0.5f);

      if(side.dir.x == 1){
        memcpy(&UVs[i*6], leftUVs, 6*sizeof(glm::vec2));
      } else {
        memcpy(&UVs[i*6], rightUVs, 6*sizeof(glm::vec2));
      }
    } else if(side.dir.z == 1 || side.dir.z == -1){
      vertices[i*6 + 0] = pos+glm::vec3(-0.5f, 0.5f,side.dir.z*0.5f);
      vertices[i*6 + 1] = pos+glm::vec3(-0.5f,-0.5f,side.dir.z*0.5f);
      vertices[i*6 + 2] = pos+glm::vec3( 0.5f,-0.5f,side.dir.z*0.5f);

      vertices[i*6 + 3] = pos+glm::vec3(-0.5f, 0.5f,side.dir.z*0.5f);
      vertices[i*6 + 4] = pos+glm::vec3( 0.5f, 0.5f,side.dir.z*0.5f);
      vertices[i*6 + 5] = pos+glm::vec3( 0.5f,-0.5f,side.dir.z*0.5f);

      if(side.dir.z == 1){
        memcpy(&UVs[i*6], frontUVs, 6*sizeof(glm::vec2));
      } else {
        memcpy(&UVs[i*6], backUVs, 6*sizeof(glm::vec2));
      }
    }
  }
  for(int i = 0;i<sidesToRender.size()*6;i++){
    glm::vec3 vertex = vertices[i];
    glm::vec2 uv = UVs[i];
  }
  if(vertexBuffer == -1){
    glGenBuffers(1,&vertexBuffer);
    glGenBuffers(1,&uvBuffer);
    glGenBuffers(1,&lightBuffer);
   }

  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sidesToRender.size()*6*sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
  glBufferData(GL_ARRAY_BUFFER, sidesToRender.size()*6*sizeof(glm::vec2), UVs.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, lightBuffer);
  glBufferData(GL_ARRAY_BUFFER, sidesToRender.size()*6*sizeof(float), lights.data(), GL_STATIC_DRAW);
}

void
Chunk::draw(glm::mat4 projection, glm::mat4 view){
  if(destroying) return;
  const std::lock_guard<std::mutex> lock(mtx);
  if(wasRecalculated) setGlBuffers();
  wasRecalculated = false;
  glm::mat4 mvp = projection*view*modelMatrix;

  glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //vertex
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glVertexAttribPointer(
    0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
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

  //light
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, lightBuffer);
  glVertexAttribPointer(
    2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
    1,                                // size
    GL_FLOAT,                         // type
    GL_FALSE,                         // normalized?
    0,                                // stride
    (void*)0                          // array buffer offset
  );

  GLuint currentTex = 0;
  GLuint currentDamageLevel = 0;
  int offset = 0;
  int length = 0;
  for(int i =0;i<sidesToRender.size();i++){
    if(sidesToRender[i].textureID != currentTex || sidesToRender[i].damageLevel != currentDamageLevel){
      if(currentTex != 0){
        glDrawArrays(GL_TRIANGLES, offset, length);
        offset += length;
        length = 0;
      }
      if(sidesToRender[i].damageLevel > 0){
        char texPath[128];
        sprintf(texPath, "textures/damageMasks/%d.dds", sidesToRender[i].damageLevel);
        GLuint maskTex = ResourceManager::getTexture(texPath);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, maskTex);
      }
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, sidesToRender[i].textureID);

      glUniform1i(3, 0);
      glUniform1i(4, 1);
      //Turn on light
      glUniform1i(6, 1);
      if(sidesToRender[i].damageLevel > 0){
        //Turn on mask
        glUniform1i(5, 1);
      } else {
        //Turn off mask
        glUniform1i(5, 0);
      }
      currentTex = sidesToRender[i].textureID;
      currentDamageLevel = sidesToRender[i].damageLevel;
    }
    length += 6;
  }

  glDrawArrays(GL_TRIANGLES, offset, length);

  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(2);

  //Turn off mask
  glUniform1i(5, 0);
  //Turn off light
  glUniform1i(6, 0);
}

void
Chunk::updateSunlight(intvec3 p){
  int maxy = p.y;
  int miny = maxy -1;
  while (!isBlock(intvec3(p.x,miny,p.z))) {
    miny--;
    if(miny < CHUNK_SIZE*-9) break;
  }
  int maxchy = floor(maxy/(float)CHUNK_SIZE);
  int minchy = floor(miny/(float)CHUNK_SIZE);
  for(int y = minchy; y<= maxchy;y++){
    intvec3 chunkPos(
      floor(p.x/(float)CHUNK_SIZE),
      y,
      floor(p.z/(float)CHUNK_SIZE)
    );
    Chunk * ch = Chunk::getChunk(chunkPos);
    if(ch == NULL) continue;
    ch->relight=true;
  }
}

void
Chunk::updateLight(bool rec){
  std::vector<intvec3> deps;
  if(useFastLight == 0){
    for(int dx = -1;dx<=1;dx++){
      for(int dy = -1;dy<=1;dy++){
        for(int dz = -1;dz<=1;dz++){
          Chunk * ch = Chunk::getChunk(pos+intvec3(dx,dy,dz));
          if(ch == NULL) continue;
          if(canSeeThrough(intvec3(dx,dy,dz)) && (!useFastLight || ch->noLight)){
            deps.push_back(pos+intvec3(dx,dy,dz));
          }

        }
      }
    }
  } else {
    deps.push_back(pos);
    useFastLight--;
  }
  int r = 0;
  for(int i = 0;i<deps.size();i++){
    Chunk * ch = Chunk::getChunk(deps[i]);
    if(ch != NULL) ch->initLight();
  }
  while(true){
    int c = 0;

    for(int i = 0;i<deps.size();i++){
      Chunk * ch = Chunk::getChunk(deps[i]);
      if(ch != NULL) c += ch->flowLight();
    }
    if(c==0)break;
  }
  for(int i = 0;i<deps.size();i++){
    Chunk * ch = Chunk::getChunk(deps[i]);
    if(ch != NULL) ch->shouldRecalculate = true;
  }
  if(useFastLight == 0) relight = false;
}

void
Chunk::initLight(){
  //Clear light from destroyed sources
  for(int x = 0; x < CHUNK_SIZE; x++){
    for(int y = 0; y < CHUNK_SIZE; y++){
      for(int z = 0; z < CHUNK_SIZE; z++){
        light_block * lbl = &light[x][y][z];
          /*if(!lbl->isSource)*/
          light[x][y][z] = light_block();
          light[x][y][z].ready = true;
          if(blocks[x][y][z] == NULL) continue;
          if(blocks[x][y][z]->isLightSource){
            light[x][y][z].value = 1.0f;
            noLight = false;
          } else {
              light[x][y][z].ready = false;
          }
      }
    }
  }
  //Light blocks directly under sky
  for(int x = 0; x < CHUNK_SIZE; x++){
      for(int z = 0; z < CHUNK_SIZE; z++){
        int y = getUnderSky(x + pos.x * CHUNK_SIZE, z + pos.z * CHUNK_SIZE) + 1;
        if(y < (pos.y + 1) *CHUNK_SIZE){
          int miny = y - pos.y*CHUNK_SIZE;
          if(miny < 0) miny = 0;
          for(int ry = miny; ry<CHUNK_SIZE;ry++){
            if(blocks[x][ry][z] != NULL) continue;
            light[x][ry][z].value = 1.0f;
            noLight = false;
          }
        }
      }
  }
  noLight = false;

}

int
Chunk::flowLight(){
          //printf("flw %d %d %d\n", pos.x, pos.y,pos.z);
  int c = 0;
  for(int x = 0; x < CHUNK_SIZE; x++){
    for(int y = 0; y < CHUNK_SIZE; y++){
      for(int z = 0; z < CHUNK_SIZE; z++){
        if(blocks[x][y][z] != NULL) continue;
        light_block lbl = light[x][y][z];
        for(int d =-1;d<=1;d+=2){
          for(int r = 0;r<3;r++){
            intvec3 p(
              r==0?d:0,
              r==1?d:0,
              r==2?d:0
            );
            p = p + intvec3(x,y,z);

            intvec3 fromChunk;
            light_block adjlbl = getLight(p, &fromChunk);

            float adjl = adjlbl.value;

            //Do not darken in y axis
            /*if(r != 1)*/ adjl -= 0.1f;
            if(adjl < 0.0f) adjl = 0.0f;
            if(adjl > lbl.value){
              if(fromChunk != pos){
                //printf("Cross Chunk dest[%d %d %d] source[%d %d %d] %f -> %f\n",pos.x,pos.y,pos.z,fromChunk.x,fromChunk.y,fromChunk.z,lbl.value,adjl);
              }
               lbl.value = adjl;
               c++;
               noLight = false;
             }
          }
        }
        light[x][y][z] = lbl;
      }
    }
  }
return c;
}

light_block
Chunk::getLight(intvec3 p, intvec3 * fromChunk){
  Chunk * ch = this;
  if(p.x<0){
    ch = Chunk::getChunk(ch->pos + intvec3(-1,0,0));
    p.x = CHUNK_SIZE+p.x;
  }
  if(ch == NULL) return light_block();
  if(p.x>=CHUNK_SIZE){
    ch = Chunk::getChunk(ch->pos + intvec3(1,0,0));
    p.x = p.x-CHUNK_SIZE;
  }
  if(ch == NULL) return light_block();

  if(p.y<0){
    ch = Chunk::getChunk(ch->pos + intvec3(0,-1,0));
    p.y = CHUNK_SIZE+p.y;
  }
  if(ch == NULL) return light_block();
  if(p.y>=CHUNK_SIZE){
    ch = Chunk::getChunk(ch->pos + intvec3(0,1,0));
    p.y = p.y-CHUNK_SIZE;
  }
  if(ch == NULL) return light_block();

  if(p.z<0){
    ch = Chunk::getChunk(ch->pos + intvec3(0,0,-1));
    p.z = CHUNK_SIZE+p.z;
  }
  if(ch == NULL) return light_block();
  if(p.z>=CHUNK_SIZE){
    ch = Chunk::getChunk(ch->pos + intvec3(0,0,1));
    p.z = p.z-CHUNK_SIZE;
  }
  if(fromChunk != NULL) *fromChunk = intvec3(0,-5000,0);
  if(ch == NULL) return light_block();
  if(fromChunk != NULL) *fromChunk = ch->pos;

  return ch->light[p.x][p.y][p.z];
}
