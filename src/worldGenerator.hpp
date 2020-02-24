#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <thread>
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
    static void startGenerator();
    static void stopGenerator();
    static void generate(glm::vec3 pos, float deltaTime);
    static unsigned long seed;
    static bool deletingChunks;
  private:
    static void generatorThWork(bool * shouldEnd, intvec3 * currentChunk);
    static void generateChunk(int chx, int chz);
    static void generateTerrain(int ** map, int tx, int tz, int xSize, int ySize);
    static void getPeaks(int tchx, int tchz, std::vector<Peak *> * peak);
    static float getRandFloat(std::mt19937 * rnd);
    static bool getHeightAt(int x, int z, int * res);
    static void addTrees(int chx, int chz);
    static void addCaves(int chx, int chz);
    static void addOres(int chx, int chz);
    static int getOreDepth(std::mt19937 * rnd);
    static bool makeHole(intvec3 pos, int chx, int chz);
    static const int terrainChunkSize = 4;
    static std::vector<intvec3> generateQueue;
    static std::thread * generator;
    static bool generatorShouldEnd;
    static intvec3 currentChunkPos;
};
