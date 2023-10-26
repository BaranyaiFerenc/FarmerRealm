#ifndef INVENTORY
#define INVENTORY

#define InventorySize 4

typedef struct Item
{
    char Name[30];
    char IconPath[50];
    unsigned int Price;
} Item;

typedef struct Slot
{
    Item item;
    unsigned short int amount;
} Slot;

int GetItemAmount(Slot* inv, int itemId);

void AddToInventory(Slot* inv, int itemId, int amount);

#endif // INVENTORY
