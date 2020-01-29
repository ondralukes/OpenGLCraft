#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "structs.hpp"

class WorldGenerator{
  public:
    static void generate(glm::vec3 pos, float deltaTime);
  private:
    static void generateTerrain(int ** map, int tx, int tz, int xSize, int ySize);
    static bool getBorderHeight(int x, int z, intvec3 dir, int length, int * res);
    static bool getHeightAt(int x, int z, int * res);
    static void placeTrees(int x, int z, int xSize, int zSize, int count);
};
