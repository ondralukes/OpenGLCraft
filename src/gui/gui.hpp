#ifndef GUI_HPP
#define GUI_HPP

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "resourceManager.hpp"
#include "guiImage.hpp"

class GUI{
public:
  GUI(GLuint mvpid, int ww, int wh);
  void draw();
private:
  int wWidth;
  int wHeight;
  GLuint vertexBuffer;
  GLuint uvBuffer;
  GLuint textureID;
  GLuint mvpID;
  GUIImage * blocks[8];
};
#endif
