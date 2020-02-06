#include "recipe.hpp"

Recipe::Recipe(){}

Recipe::Recipe(const Blocks::block_type b[][3], Blocks::block_type o, int c){
  for(int x = 0;x < 3;x++){
    for(int y = 0;y < 3;y++){
      blocks[x][y] = b[y][x];
    }
  }
  output.type = o;
  output.dataPos = 0;
  count = c;
}

bool
Recipe::doesMatch(Blocks::block_data b[][3]){
  for(int x = 0;x < 3;x++){
    for(int y = 0;y < 3;y++){
      if(blocks[x][y] != b[x][y].type) return false;
    }
  }
  return true;
}

Blocks::block_data
Recipe::getOutput(){
  return output;
}

int
Recipe::getCount(){
  return count;
}
