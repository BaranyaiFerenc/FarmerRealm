#ifndef GAMEELEMENTS_H
#define GAMEELEMENTS_H

#include "Graphics.h"
#include "ProcessHandler.h"
#include "debugmalloc.h"

typedef struct Tile //Alap csempe struktúra ezekre épül a játék
{
    unsigned short int id;
    char name[20];
    Vector2 positionToMouse; //A csempe pozíciója relatív az egérhez, ez kell hogy mozgatásnál ne ugorjon oda az egérhez hanem tartsa a relatív helyzetet
    Vector2 coordinates;
    Image img;
    Image additionalImage;
    Image icon;
    Process *process;
    bool arrow;
} Tile;

typedef struct TileMatrix
{
    Tile** matrix;
    int xSize;
    int ySize;
} TileMatrix;


typedef struct Item
{
    char Name[100];
    unsigned int Price;
    unsigned int Amount;
    unsigned int id;
} Item;

typedef enum RecipeType{Windmill, Bakery, Brewery} RecipeType;

typedef struct Recipe
{
    int itemIds[2];
    int itemAmounts[2];

    int resultId;
    int resultAmount;

    RecipeType type;

    unsigned int t;
    unsigned int id;
} Recipe;

typedef struct ArrayData
{
    void *arr;
    int n;
} ArrayData;

Tile** CreateMatrix(int xSize, int ySize);
/*Mátrix létrehozása*/

void FreeMatrix(TileMatrix *matrix);
/*Mátrix felszabadítása*/

Tile* GetTileFromPosition(TileMatrix *matrix, Vector2 position);
/*Keresés a mátrixban pozíció alapján*/

Tile* GetTileByID(TileMatrix *matrix, int id);
/*Keresés a mátrixban azonosító szerint*/

Item* GetItemByName(char *name, int n, Item *itemList);
/*Tárgy megkeresése az inventoryban*/

#endif
