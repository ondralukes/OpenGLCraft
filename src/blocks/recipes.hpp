#ifndef RECIPES_HPP
#define RECIPES_HPP

#include "block.hpp"
#include "recipe.hpp"

using namespace Blocks;

class Recipes {
public:
  static const int recipesCount = 3;
  static Recipe recipes[3];
  static void init();
};

#endif
