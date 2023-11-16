#ifndef GAMEELEMENTS_H
#define GAMEELEMENTS_H

#include "Graphics.h"
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

Tile** CreateMatrix(int xSize, int ySize);

void FreeMatrix(TileMatrix *matrix);

Tile* GetTileFromPosition(TileMatrix *matrix, Vector2 position);

Tile* GetTileByID(TileMatrix *matrix, int id);

#endif
