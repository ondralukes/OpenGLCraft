#ifndef CHEST_GUI_HPP
#define CHEST_GUI_HPP

#include "itemField.hpp"
#include "blocks/chest.hpp"
#include "blocks/recipe.hpp"

using namespace GUIUtils;

namespace Blocks {
class Chest;
}

namespace GUIUtils {
  class ChestGUI : public GUIBase {
  public:
    ChestGUI(Blocks::Chest * ch);
    void generateFields(ItemField ** f);
    void leaveGUI();
    void drawExtras();
    Blocks::Chest * chest;
  };
}
#endif
