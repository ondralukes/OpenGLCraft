#ifndef FURNACE_GUI_HPP
#define FURNACE_GUI_HPP

#include "itemField.hpp"
#include "blocks/furnace.hpp"
#include "blocks/recipe.hpp"

namespace Blocks {
class Furnace;
}

using namespace GUIUtils;

namespace GUIUtils {
  class FurnaceGUI : public GUIBase {
  public:
    FurnaceGUI(Blocks::Furnace * f);
    void generateFields(ItemField ** f);
    void leaveGUI();
    void drawExtras();
    static void _loadFuel();
    void loadFuel();
    static void _loadTarget();
    static void _loadTarget(int count);
    void loadTarget();
  private:
    Blocks::Furnace * furnace;
  };
}
#endif
