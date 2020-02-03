#ifndef GUI_HPP
#define GUI_HPP

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "resourceManager.hpp"
#include "guiImage.hpp"
#include "itemStack.hpp"
#include "blocks/block.hpp"
#include "inventory.hpp"
#include "text.hpp"

class GUI{
public:
  static void init(GLuint mvpid, int ww, int wh);
  static void draw(glm::vec2 mousePos);
  static void refresh();
  static int selectedItemIndex;
  static void dispose();
  static void leftMouseButton(glm::vec2 mousePos, bool state);
  static void leaveGUI();
private:
  static int wWidth;
  static int wHeight;
  static GLuint vertexBuffer;
  static GLuint uvBuffer;
  static GLuint textureID;
  static GLuint selectedTextureID;
  static GLuint mvpID;
  static GLuint shaderID;
  static ItemStack * blocks[8];
  static GUIImage * crosshair;
  static TextManager * textManager;
};
#endif
