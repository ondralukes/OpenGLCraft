#include "gui.hpp"

GUI::GUI(GLuint mvpid, int ww, int wh){
  static const float UVs[]{
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
  };
  glGenBuffers(1,&vertexBuffer);

  glGenBuffers(1,&uvBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(UVs), UVs, GL_STATIC_DRAW);
  textureID = ResourceManager::getTexture("textures/itembar.dds");
  mvpID = mvpid;
  wWidth = ww;
  wHeight = wh;

  float bottomMargin = 0.05f;
  float totalWidth = 1.6f;
  float cellWidth = totalWidth/8.0f;
  float cellHeight = cellWidth * wWidth / (float)wHeight;
  for(int i =0;i<8;i++){
    float xStart = -0.8f+i*0.2f;
    float xEnd = -0.6f+i*0.2f;
    float yStart = -1.0f + bottomMargin;
    float yEnd =  -1.0f + bottomMargin + cellHeight;
    xStart += cellWidth *0.2f;
    xEnd -= cellWidth *0.2f;
    yStart += cellHeight *0.2f;
    yEnd -= cellHeight *0.2f;
    blocks[i] = new GUIImage(mvpID, ResourceManager::getTexture("textures/grass.dds"),glm::vec4(xStart,yStart,xEnd,yEnd));
  }
}

void
GUI::draw(){
  glm::mat4 mvp = glm::mat4(1.0f);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glUniform1i(textureID, 0);

  float bottomMargin = 0.05f;
  float totalWidth = 1.6f;
  float cellWidth = totalWidth/8.0f;
  float cellHeight = cellWidth * wWidth / (float)wHeight;
  for(int i =0;i<8;i++){
    float xStart = -0.8f+i*0.2f;
    float xEnd = -0.6f+i*0.2f;
    float yStart = -1.0f + bottomMargin;
    float yEnd =  -1.0f + bottomMargin + cellHeight;
    float vertices[]{
       xStart, yStart, 0.0f,
       xEnd, yEnd, 0.0f,
       xStart, yEnd, 0.0f,
       xStart, yStart, 0.0f,
       xEnd, yStart, 0.0f,
       xEnd, yEnd, 0.0f,
    };
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(glm::vec3), vertices, GL_STATIC_DRAW);
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

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
  }

  for(int i =0;i<8;i++){
    blocks[i]->draw();
  }
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
}
