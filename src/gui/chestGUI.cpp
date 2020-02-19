#include "chestGUI.hpp"

ChestGUI::ChestGUI(Blocks::Chest * ch){
  itemFieldsCount = 24;
  chest = ch;
}

void
ChestGUI::generateFields(ItemField ** f){
  fields = f;
  for(int x = 0;x < 8;x++){
    for(int y = 0;y < 3;y++){
      fields[x+y*8] = new ItemField(glm::vec2(192 + x*128, 227 + (2-y)*128));
      fields[x+y*8]->drop = false;
      inventory_item content = chest->getContent(x+y*8);
      if(content.block != NULL && content.count != 0){
        GUI::blocks.push_back(
          new ItemStack(
            GUI::mvpID,
            Blocks::Block::getTextureFor(content.block->getType()),
            GUI::shaderID,
            glm::vec4 (0,0,100,100),
            GUI::textManager,
            content.block
          )
        );
      int index = GUI::blocks.size() - 1;
      GUI::blocks[index] -> setCount(content.count);
      glm::vec2 p = fields[x+y*8]->getPosition();
      GUI::blocks[index]->setPosition(
        glm::vec4(
          p.x - 76.8f/2.0f,
          p.y - 76.8f/2.0f,
          p.x + 76.8f/2.0f,
          p.y + 76.8f/2.0f
        )
      );
      fields[x+y*8]->put(index, false, true);
      }
    }
  }
}

void
ChestGUI::leaveGUI(){
  for(int x = 0;x < 8;x++){
    for(int y = 0;y < 3;y++){
      ItemField * f = fields[x+y*8];
      if(f == NULL) continue;
      inventory_item item;
      ItemStack * c = f->getContent();
      if(c != 0){
        item.count = c->getCount();
        item.block = c->getBlock();
      }
      chest->setContent(x+y*8, item);
    }
  }
  chest->save();
}

void
ChestGUI::drawExtras(){
  
}
