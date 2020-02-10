#include "inventory.hpp"

inventory_item Inventory::inventory[8];

void
Inventory::add(Blocks::Block * block, int count){
  int emptySlotIndex = -1;
  for(int i = 0;i<8;i++){
    if(inventory[i].block == NULL){
      if(emptySlotIndex == -1) emptySlotIndex = i;
    } else if(inventory[i].block->getType() == block->getType() && inventory[i].count != block->maxStack){
      int c = block->maxStack - inventory[i].count;
      if(c > count) c = count;
      inventory[i].count += c;
      if(c < count){
        add(block, count - c);
      }
      SaveManager::main->saveInventory();
      GUI::reload();
      return;
    }
  }
  if(emptySlotIndex != -1){
    inventory[emptySlotIndex].block = block;
    inventory[emptySlotIndex].count = count;
  }
  GUI::reload();
  SaveManager::main->saveInventory();
}

bool
Inventory::isPlaceFor(Blocks::block_data block){
  for(int i = 0;i<8;i++){
    if(inventory[i].block == NULL){
      return true;
    } else if(inventory[i].block->getType() == block.type){
      if(inventory[i].count < inventory[i].block->maxStack){
        return true;
      }
    }
  }
  return false;
}

void
Inventory::remove(int index, int count){
  inventory[index].count -= count;
  if(inventory[index].count <= 0){
    inventory[index].block = NULL;
    inventory[index].count = 0;
  }
  GUI::reload();
  SaveManager::main->saveInventory();
}

Blocks::Block *
Inventory::getSelectedBlock(){
  return inventory[GUI::selectedItemIndex].block;
}

int
Inventory::getSelectedCount(){
  return inventory[GUI::selectedItemIndex].count;
}

void
Inventory::destroySelectedBlock(bool del){
  if(getSelectedBlock() == NULL) return;
  inventory[GUI::selectedItemIndex].block->doDrop = false;
  if(del) delete inventory[GUI::selectedItemIndex].block;
  inventory[GUI::selectedItemIndex].block = NULL;
}
