#ifndef RECIPES_HPP
#define RECIPES_HPP

#include "block.hpp"
#include "recipe.hpp"

using namespace Blocks;

class Recipes {
public:
  static const int recipesCount = 10;
  static Recipe recipes[10];
  static void init();
};

#endif
