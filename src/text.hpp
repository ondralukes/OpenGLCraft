#include <vector>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "loadShaders.hpp"
#include "resourceManager.hpp"

struct textchar_t{
  GLuint texID;
  glm::vec2 sz;
  glm::vec2 bear;
  GLuint adv;
};

class TextManager {
  public:
    void init(const char * fontPath);
    void drawText(const char * text, glm::vec2 pos, float size);
  private:
    GLuint shaderID;
    GLuint textureID;
    GLuint verticesBuffer;
    GLuint UVBuffer;
    GLuint uniformID;
    textchar_t textChars[128];
};
