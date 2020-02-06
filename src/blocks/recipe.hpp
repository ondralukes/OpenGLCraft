#ifndef RECIPE_HPP
#define RECIPE_HPP

#include "block.hpp"
class Recipe {
public:
  Recipe();
  Recipe(const Blocks::block_type b[][3], Blocks::block_type o, int c);
  bool doesMatch(Blocks::block_data b[][3]);
  int getCount();
  Blocks::block_data getOutput();
private:
  Blocks::block_type blocks[3][3];
  Blocks::block_data output;
  int count;
};

#endif
