#ifndef GUI_HPP
#define GUI_HPP

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "resourceManager.hpp"
#include "guiImage.hpp"
#include "guiBase.hpp"
#include "craftingGUI.hpp"
#include "chestGUI.hpp"
#include "itemStack.hpp"
#include "itemField.hpp"
#include "blocks/blocks.hpp"
#include "inventory.hpp"
#include "text.hpp"
#include "droppedBlock.hpp"
#include "blocks/recipes.hpp"

using namespace GUIUtils;

class ItemField;

enum gui_type {
  GUI_CRAFTING,
  GUI_CHEST
};

class GUI{
public:
  static void init(GLuint mvpid, int ww, int wh);
  static void draw(glm::vec2 mousePos);
  static void reload();
  static int selectedItemIndex;
  static void dispose();
  static void mouseButton(glm::vec2 mousePos, bool right, bool state, glm::vec3 pos, glm::vec3 dir);
  static void leaveGUI(glm::vec3 pos, glm::vec3 dir);
  static void enterGUI(gui_type type = GUI_CRAFTING, Blocks::Block * bl = NULL);
  static void dropSelected(glm::vec3 pos, glm::vec3 dir);
  static std::vector<ItemStack *> blocks;
  static GLuint mvpID;
  static GLuint shaderID;
  static TextManager * textManager;
  static GUIBase * currentGUI;
  static bool inGUI;
private:
  static void updateInventory();
  static int wWidth;
  static int wHeight;
  static GLuint vertexBuffer;
  static GLuint uvBuffer;
  static GLuint textureID;
  static GLuint selectedTextureID;
  static GUIImage * crosshair;
  static int itemFieldsCount;
  static ItemField ** itemFields;
  static int currentDraggingIndex;
  static int craftingOutputIndex;
};
#endif
