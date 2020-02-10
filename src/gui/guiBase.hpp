#ifndef GUI_BASE_HPP
#define GUI_BASE_HPP

#include "itemField.hpp"

class ItemField;

namespace GUIUtils {
  class GUIBase {
  public:
    virtual void enterGUI() = 0;
    virtual void leaveGUI() = 0;
    virtual void mouseButton(glm::vec2 mousePos, bool right, bool state, glm::vec3 pos, glm::vec3 dir) = 0;
    virtual void generateFields(ItemField * fields) = 0;
    int itemFieldsCount = 0;
  };
}
#endif
