#include "craftingGUI.hpp"

CraftingGUI::CraftingGUI(){
  itemFieldsCount = 10;
}

void
CraftingGUI::leaveGUI(){

}

void
CraftingGUI::generateFields(ItemField ** f){
  fields = f;
  for(int x = 0;x < 3;x++){
    for(int y = 0;y < 3;y++){
      fields[x+y*3] = new ItemField(glm::vec2(507 + x*133, 227 + (2-y)*133));
      fields[x+y*3]->onPut = &_update;
      fields[x+y*3]->onGet = &_update;
    }
  }
  fields[9] = new ItemField(glm::vec2(1039, 360));
  fields[9]->getOnly = true;
  fields[9]->onGet = &_craft;
  fields[9]->drop = false;
}

void CraftingGUI::_update(){ ((CraftingGUI *) GUI::currentGUI)->update(); }
void CraftingGUI::_update(int count){ ((CraftingGUI *) GUI::currentGUI)->update(); }
void
CraftingGUI::update(){
  if(fields[9]->getContent() != NULL){
    fields[9]->empty();
  }
  Blocks::block_data input[3][3];
  int count = 999999999;
  bool empty = true;
  for(int x = 0;x < 3;x++){
    for(int y = 0;y < 3;y++){
      if(fields[x+y*3]->getContent() == NULL){
        input[x][y].type = Blocks::NONE;
        input[x][y].dataPos = 0;
      } else {
        input[x][y] = fields[x+y*3]->getContent()->getBlock()->getBlockData();
        int c = fields[x+y*3]->getContent()->getCount();
        if(c < count) count = c;
        empty = false;
      }
    }
  }
  if(empty) return;
  //Move to left
  while(true){
    bool move = true;
    for(int y = 0;y<3;y++){
      if(input[0][y].type != Blocks::NONE) move = false;
    }
    if(!move) break;
    for(int x = 0;x<2;x++){
      for(int y = 0;y < 3;y++){
        input[x][y] = input[x+1][y];
        input[x+1][y].type = Blocks::NONE;
        input[x+1][y].dataPos = 0;
      }
    }
  }

  //Move to top corner
  while(true){
    bool move = true;
    for(int x = 0;x<3;x++){
      if(input[x][0].type != Blocks::NONE) move = false;
    }
    if(!move) break;
    for(int y = 0;y<2;y++){
      for(int x = 0;x < 3;x++){
        input[x][y] = input[x][y+1];
        input[x][y+1].type = Blocks::NONE;
        input[x][y+1].dataPos = 0;
      }
    }
  }

  int craftingOutputIndex;
  bool foundRecipe = false;
  for(int i =0;i<Recipes::recipesCount;i++){
    if(Recipes::recipes[i].doesMatch(input)){
      Blocks::Block * block = Blocks::Block::decodeBlock(Recipes::recipes[i].getOutput(), intvec3(0,0,0), 0);
      block->doDrop = false;
      GUI::blocks.push_back(new ItemStack(
        GUI::mvpID,
        Blocks::Block::getTextureFor(block->getType()),
        GUI::shaderID,
        glm::vec4(1000.6f, 321.6f, 1077.4f, 398.4f),
        GUI::textManager,
        block
      ));
      craftingOutputIndex = GUI::blocks.size() - 1;
      count = count *  Recipes::recipes[i].getCount();
      if(count > block->maxStack) count = block->maxStack;
      GUI::blocks[craftingOutputIndex]->setCount(count);
      recipe = &Recipes::recipes[i];
      fields[9]->put(craftingOutputIndex, false, true);
      fields[9]->minGet = recipe->getCount();
      foundRecipe = true;
      break;
    }
  }
}

void CraftingGUI::_craft(int count){ ((CraftingGUI *) GUI::currentGUI)->craft(count); }
void
CraftingGUI::craft(int count){
  count /= recipe->getCount();
  for(int x = 0;x < 3;x++){
    for(int y = 0;y < 3;y++){
      if(fields[x+y*3]->getContent() == NULL) continue;
      int c = fields[x+y*3]->getContent()->getCount();
      fields[x+y*3]->getContent()->setCount(c-count);
      if(c - count <= 0){
        fields[x+y*3]->empty();
      }
    }
  }
  update();
}

void
CraftingGUI::drawExtras(){

}
