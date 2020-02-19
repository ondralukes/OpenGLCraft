#include "furnaceGUI.hpp"

FurnaceGUI::FurnaceGUI(Blocks::Furnace * f){
  itemFieldsCount = 3;
  furnace = f;
}

void
FurnaceGUI::leaveGUI(){
  for(int i =0;i<3;i++){
    ItemField * f = fields[i];
    if(f == NULL) continue;
    inventory_item item;
    ItemStack * c = f->getContent();
    if(c != 0){
      item.count = c->getCount();
      item.block = c->getBlock();
    }
    furnace->content[i] = item;
  }
  furnace->save();
}

void
FurnaceGUI::generateFields(ItemField ** f){
  fields = f;
  fields[0] = new ItemField(glm::vec2(640, 493));
  fields[0]->drop = false;
  fields[0]->onPut = &_loadTarget;
  fields[0]->onGet = &_loadTarget;

  fields[1] = new ItemField(glm::vec2(640, 227));
  fields[1]->onPut = &_loadFuel;
  fields[1]->drop = false;

  fields[2] = new ItemField(glm::vec2(906, 360));
  fields[2]->getOnly = true;
  fields[2]->onGet = &_loadTarget;
  fields[2]->drop = false;

  for(int i = 0;i<3;i++){
    if(furnace->content[i].count != 0 && furnace->content[i].block != NULL){
      inventory_item content = furnace->content[i];
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
    glm::vec2 p = fields[i]->getPosition();
    GUI::blocks[index]->setPosition(
      glm::vec4(
        p.x - 76.8f/2.0f,
        p.y - 76.8f/2.0f,
        p.x + 76.8f/2.0f,
        p.y + 76.8f/2.0f
      )
    );
    fields[i]->put(index, false, true);
    }
  }
}

void
FurnaceGUI::drawExtras(){
  float oldFuel;
  do{
    furnace->update();
    oldFuel = furnace->fuel;
    if(furnace->fuel < 0.05f) loadFuel();
  } while(oldFuel != furnace->fuel);
  if(fields[0] != NULL){
    ItemStack * content = fields[0]->getContent();
    if(content != NULL){
      if(content->getBlock() != NULL){
        while(furnace->blockFuelBurnt >= content->getBlock()->meltFuel){
          furnace->blockFuelBurnt -= content->getBlock()->meltFuel;
          bool last = false;
          if(content->getCount() > 1){
            content->setCount(content->getCount()-1);
          } else {
            last = true;
          }
          //Create output stack (if not exists)
          bool createResult = false;
          if(fields[2] == NULL) createResult = true; else
          if(fields[2]->getContent() == NULL) createResult = true; else
          if(fields[2]->getContent()->getCount() == 0) createResult = true;
          if(createResult){
            block_type blockType = content->getBlock()->meltsTo;
            block_data blockData = block_data();
            blockData.type = blockType;
            blockData.dataPos = 0;
            GUI::blocks.push_back(
              new ItemStack(
                GUI::mvpID,
                Blocks::Block::getTextureFor(blockType),
                GUI::shaderID,
                glm::vec4 (0,0,100,100),
                GUI::textManager,
                Blocks::Block::decodeBlock(blockData, intvec3(0,0,0), GUI::mvpID)
              )
            );
          int index = GUI::blocks.size() - 1;
          GUI::blocks[index] -> setCount(0);
          glm::vec2 p = fields[2]->getPosition();
          GUI::blocks[index]->setPosition(
            glm::vec4(
              p.x - 76.8f/2.0f,
              p.y - 76.8f/2.0f,
              p.x + 76.8f/2.0f,
              p.y + 76.8f/2.0f
            )
          );
          fields[2]->put(index, false, true);
          }
          fields[2]->getContent()->setCount(fields[2]->getContent()->getCount()+1);
          loadTarget();
          if(last){
            fields[0]->empty();
            break;
          }
        }
      }
    }
  }

  if(fields[0] != NULL){
    ItemStack * content = fields[0]->getContent();
    if(content != NULL){
      if(content->getBlock() != NULL){
        char text[32];
        sprintf(text, "%.2f%%",furnace->blockFuelBurnt/content->getBlock()->meltFuel*100.0f);
        GUI::textManager->drawText(text, glm::vec2(640, 360), 28.0f);
      }
    }
  }
}

void FurnaceGUI::_loadFuel(){ ((FurnaceGUI *) GUI::currentGUI)->loadFuel(); }
void
FurnaceGUI::loadFuel(){
  if(fields[1] == NULL) return;
  ItemStack * content = fields[1]->getContent();
  if(content == NULL) return;
  if(content->getCount() != 0){
    while(true){
    if(content->getBlock()->fuelValue != 0.0f && (1.0f-furnace->fuel) >= content->getBlock()->fuelValue){
      furnace->fuel += content->getBlock()->fuelValue;
      if(content->getCount() > 1){
        content->setCount(content->getCount()-1);
      } else {
        fields[1]->empty();
        furnace->burning = false;
        break;
      }
    } else {
      break;
    }
  }
  }
  loadTarget();
}

void FurnaceGUI::_loadTarget(){ ((FurnaceGUI *) GUI::currentGUI)->loadTarget(); }
void FurnaceGUI::_loadTarget(int count){ ((FurnaceGUI *) GUI::currentGUI)->loadTarget(); }
void
FurnaceGUI::loadTarget(){
  if(fields[0] == NULL) return;
  ItemStack * content = fields[0]->getContent();
  if(content == NULL){
    furnace->burning = false;
    return;
  }
  if(content->getCount() != 0){
    if(content->getBlock()->meltsTo != NONE){
      furnace->burning = true;
      if(fields[2] != NULL){
        if(fields[2]->getContent() != NULL){
          if(fields[2]->getContent()->getCount() != 0){
            if(fields[2]->getContent()->getBlock()->getType() != content->getBlock()->meltsTo ||
            fields[2]->getContent()->getCount() == fields[2]->getContent()->getBlock()->maxStack){
              furnace->burning = false;
            }
          }
        }
      }
    } else {
      furnace->burning = false;
    }
  } else {
    furnace->burning = false;
  }
  if(!furnace->burning) furnace->blockFuelBurnt = 0.0f;
}
