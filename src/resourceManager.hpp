#ifndef RES_MGR_H
#define RES_MGR_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>

#include "loadObj.hpp"
#include "loadTexture.hpp"

struct texture_t {
  GLuint id;
  const char * name;
};

struct obj_t {
  std::vector<glm::vec3> * vertices;
  std::vector<glm::vec2> * uvs;
  const char * name;
};

class ResourceManager{
  public:
    static GLuint getTexture(const char * path, bool filtering = true);
    static std::vector<texture_t> textures;
    static std::vector<glm::vec3> * getObjVertices(const char * path);
    static std::vector<glm::vec2> * getObjUVs(const char * path);
    static std::vector<obj_t> objs;
};

#endif
