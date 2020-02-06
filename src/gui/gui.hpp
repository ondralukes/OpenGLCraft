#ifndef GUI_HPP
#define GUI_HPP

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "resourceManager.hpp"
#include "guiImage.hpp"
#include "itemStack.hpp"
#include "itemField.hpp"
#include "blocks/block.hpp"
#include "inventory.hpp"
#include "text.hpp"
#include "droppedBlock.hpp"
#include "blocks/recipes.hpp"

class ItemField;

class GUI{
public:
  static void init(GLuint mvpid, int ww, int wh);
  static void draw(glm::vec2 mousePos);
  static void refresh();
  static int selectedItemIndex;
  static void dispose();
  static void mouseButton(glm::vec2 mousePos, bool right, bool state, glm::vec3 pos, glm::vec3 dir);
  static void leaveGUI(glm::vec3 pos, glm::vec3 dir);
  static void enterGUI();
  static void dropSelected(glm::vec3 pos, glm::vec3 dir);
  static std::vector<ItemStack *> blocks;
private:
  static int wWidth;
  static int wHeight;
  static GLuint vertexBuffer;
  static GLuint uvBuffer;
  static GLuint textureID;
  static GLuint selectedTextureID;
  static GLuint mvpID;
  static GLuint shaderID;
  static GUIImage * crosshair;
  static GUIImage * craftingGrid;
  static TextManager * textManager;
  static bool inGUI;
  static ItemField * itemFields;
  static int currentDraggingIndex;
  static int craftingOutputIndex;

  static void updateCraftingResult();
};
#endif
