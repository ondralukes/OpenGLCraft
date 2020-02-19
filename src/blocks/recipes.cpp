#include "recipes.hpp"

Recipe Recipes::recipes[10];

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

  //Wooden axe recipe
  {
    Blocks::block_type in[3][3] = {
      {WOOD, WOOD, NONE},
      {STICK, WOOD, NONE},
      {STICK, NONE , NONE}
    };
    recipes[3] = Recipe(in, WOODEN_AXE, 1);
  }

  //Stone axe recipe
  {
    Blocks::block_type in[3][3] = {
      {STONE, STONE, NONE},
      {STICK, STONE, NONE},
      {STICK, NONE , NONE}
    };
    recipes[4] = Recipe(in, STONE_AXE, 1);
  }

  //Wooden shovel recipe
  {
    Blocks::block_type in[3][3] = {
      {WOOD, NONE, NONE},
      {STICK, NONE, NONE},
      {STICK, NONE , NONE}
    };
    recipes[5] = Recipe(in, WOODEN_SHOVEL, 1);
  }

  //Stone axe recipe
  {
    Blocks::block_type in[3][3] = {
      {STONE, NONE, NONE},
      {STICK, NONE, NONE},
      {STICK, NONE , NONE}
    };
    recipes[6] = Recipe(in, STONE_SHOVEL, 1);
  }

  //Chest recipe
  {
    Blocks::block_type in[3][3] = {
      {WOOD, WOOD, WOOD},
      {WOOD, NONE, WOOD},
      {WOOD, WOOD, WOOD}
    };
    recipes[7] = Recipe(in, CHEST, 1);
  }

  //Fire recipe
  {
    Blocks::block_type in[3][3] = {
      {WOOD, NONE, NONE},
      {STICK, NONE, NONE},
      {NONE, NONE, NONE}
    };
    recipes[8] = Recipe(in, FIRE, 16);
  }
  //Furnace recipe
  {
    Blocks::block_type in[3][3] = {
      {STONE, STONE, STONE},
      {STONE, NONE, STONE},
      {STONE, STONE, STONE}
    };
    recipes[9] = Recipe(in, FURNACE, 1);
  }
}
