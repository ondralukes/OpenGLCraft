#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <random>

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "structs.hpp"

namespace WorldGeneratorUtils{
  class Peak {
  public:
    Peak(int _x, int _z, float _a, float _c)
      :x(_x), z(_z), a(_a), c(_c) {};
    int x;
    int z;
    float a;
    float c;
    float getValueAt(int x, int z);
  };
}

using namespace WorldGeneratorUtils;

class WorldGenerator{
  public:
    static void generate(glm::vec3 pos, float deltaTime);
  private:
    static void generateTerrain(int ** map, int tx, int tz, int xSize, int ySize);
    static void getPeaks(int tchx, int tchz, std::vector<Peak *> * peak);
    static float getRandFloat(std::mt19937 * rnd);
    static const int terrainChunkSize = 4;
};
