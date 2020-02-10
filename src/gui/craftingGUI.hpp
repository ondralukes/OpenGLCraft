#ifndef CRAFTING_GUI_HPP
#define CRAFTING_GUI_HPP

#include "itemField.hpp"
#include "blocks/recipe.hpp"

using namespace GUIUtils;

namespace GUIUtils {
  class CraftingGUI : public GUIBase {
  public:
    CraftingGUI();
    void generateFields(ItemField ** f);
  private:
    static void _update();
    void update();
    static void _craft(int count);
    void craft(int count);
    Recipe * recipe;
  };
}
#endif
