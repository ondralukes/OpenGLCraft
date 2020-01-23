#ifndef GUI_IMAGE_HPP
#define GUI_IMAGE_HPP

#include <cstdio>
#include <GL/glew.h>
#include <glm/glm.hpp>

class GUIImage{

public:
  GUIImage(GLuint mvpid, GLuint texID, glm::vec4 pos);
  void draw();
private:
  GLuint textureID;
  GLuint vertexBuffer;
  GLuint uvBuffer;
  GLuint mvpID;
};
#endif
