#ifndef GUI_HPP
#define GUI_HPP

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "resourceManager.hpp"
#include "guiImage.hpp"
#include "blocks/block.hpp"
#include "inventory.hpp"
#include "text.hpp"

class GUI{
public:
  static void init(GLuint mvpid, int ww, int wh);
  static void draw();
  static void refresh();
private:
  static int wWidth;
  static int wHeight;
  static GLuint vertexBuffer;
  static GLuint uvBuffer;
  static GLuint textureID;
  static GLuint mvpID;
  static GUIImage * blocks[8];
  static TextManager * textManager;
};
#endif
