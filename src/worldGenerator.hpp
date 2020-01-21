#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

#include <glm/glm.hpp>
#include <GL/glew.h>



class WorldGenerator{
  public:
    static void generate(glm::vec3 pos, float deltaTime);
};
