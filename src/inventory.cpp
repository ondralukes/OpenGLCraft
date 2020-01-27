#include "inventory.hpp"

inventory_item Inventory::inventory[8];
SaveManager * Inventory::saveManager;

void
Inventory::add(Blocks::block_type type, int count){
  int emptySlotIndex = -1;
  for(int i = 0;i<8;i++){
    if(inventory[i].type == type){
      inventory[i].count += count;
      GUI::refresh();
      return;
    } else if(inventory[i].type == Blocks::NONE){
      if(emptySlotIndex == -1) emptySlotIndex = i;
    }
  }
  if(emptySlotIndex != -1){
    inventory[emptySlotIndex].type = type;
    inventory[emptySlotIndex].count = count;
  }
  GUI::refresh();
  saveManager->saveInventory();
}

void
Inventory::remove(int index, int count){
  inventory[index].count -= count;
  if(inventory[index].count <= 0){
    inventory[index].type = Blocks::NONE;
    inventory[index].count = 0;
  }
  GUI::refresh();
}

Blocks::block_type
Inventory::getSelectedBlock(){
  return inventory[GUI::selectedItemIndex].type;
}
