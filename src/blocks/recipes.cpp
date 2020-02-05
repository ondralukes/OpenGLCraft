#include "recipes.hpp"

Recipe Recipes::recipes[3];

void
Recipes::init(){
  //Stick recipe
  {
    Blocks::block_type in[3][3] = {
      {WOOD, NONE, NONE},
      {WOOD, NONE, NONE},
      {NONE, NONE, NONE}
    };
    recipes[0] = Recipe(in, STICK, 16);
  }

  //Wooden pickaxe recipe
  {
    Blocks::block_type in[3][3] = {
      {WOOD, WOOD, WOOD},
      {NONE, STICK, NONE},
      {NONE, STICK, NONE}
    };
    recipes[1] = Recipe(in, WOODEN_PICKAXE, 1);
  }

  //Stone pickaxe recipe
  {
    Blocks::block_type in[3][3] = {
      {STONE, STONE, STONE},
      {NONE, STICK, NONE},
      {NONE, STICK, NONE}
    };
    recipes[2] = Recipe(in, STONE_PICKAXE, 1);
  }
}
