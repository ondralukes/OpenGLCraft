#include "recipes.hpp"

Recipe Recipes::recipes[1];

void
Recipes::init(){
  Blocks::block_type in[3][3] = {
    {WOOD, NONE, NONE},
    {WOOD, NONE, NONE},
    {NONE, NONE, NONE}
  };
  recipes[0] = Recipe(in, STICK, 16);
}
