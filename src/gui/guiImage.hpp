#ifndef GUI_IMAGE_HPP
#define GUI_IMAGE_HPP

#include <cstdio>
#include <GL/glew.h>
#include <glm/glm.hpp>

class GUIImage{

public:
  GUIImage(GLuint mvpid, GLuint texID, GLuint _shaderID, glm::vec4 _pos, bool isBlock = false);
  virtual void draw();
  void setTexture(GLuint texID);
  GLuint getTexture();
  void setPosition(glm::vec4 _pos);
  glm::vec4 getPosition();
protected:
  glm::vec4 pos;
  GLuint textureID;
  GLuint vertexBuffer;
  GLuint uvBuffer;
  GLuint mvpID;
  GLuint shaderID;
};
#endif
