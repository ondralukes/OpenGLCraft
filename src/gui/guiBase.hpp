#ifndef GUI_BASE_HPP
#define GUI_BASE_HPP

#include "itemField.hpp"

class ItemField;

namespace GUIUtils {
  class GUIBase {
  public:
    virtual void generateFields(ItemField ** f) = 0;
    virtual void leaveGUI() = 0;
    virtual void drawExtras() = 0;
    int itemFieldsCount = 0;
  protected:
    ItemField ** fields;
  };
}
#endif
