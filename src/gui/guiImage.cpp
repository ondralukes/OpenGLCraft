#include "guiImage.hpp"

GUIImage::GUIImage(GLuint mvpid, GLuint texID, GLuint _shaderID, glm::vec4 _pos){
  pos = _pos;

  const static float UVs[]{
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
  };


  glGenBuffers(1,&vertexBuffer);

  //Puts data to vertexBuffer
  setPosition(pos);

  glGenBuffers(1,&uvBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(UVs), UVs, GL_STATIC_DRAW);
  textureID = texID;
  mvpID = mvpid;
  shaderID = _shaderID;
}

glm::vec4
GUIImage::getPosition(){
  return pos;
}

void
GUIImage::setPosition(glm::vec4 _pos){
  pos = _pos;
  float vertices[]{
    pos.x, pos.y, 0.0f,
    pos.z, pos.w, 0.0f,
    pos.x, pos.w, 0.0f,
    pos.x, pos.y, 0.0f,
    pos.z, pos.y, 0.0f,
    pos.z, pos.w, 0.0f
  };

  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void
GUIImage::setTexture(GLuint texID){
  textureID = texID;
}

GLuint
GUIImage::getTexture(){
  return textureID;
}

void
GUIImage::draw(){
  glm::mat4 mvp = glm::mat4(1.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);

  glUseProgram(shaderID);

  glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glUniform1i(textureID, 0);

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

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
}
