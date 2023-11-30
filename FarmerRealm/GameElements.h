#ifndef GAMEELEMENTS_H
#define GAMEELEMENTS_H

#include "Graphics.h"
#include "ProcessHandler.h"
#include "debugmalloc.h"

typedef struct Tile //Alap csempe strukt�ra ezekre �p�l a j�t�k
{
    unsigned short int id;
    char name[20];
    Vector2 positionToMouse; //A csempe poz�ci�ja relat�v az eg�rhez, ez kell hogy mozgat�sn�l ne ugorjon oda az eg�rhez hanem tartsa a relat�v helyzetet
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
/*M�trix l�trehoz�sa*/

void FreeMatrix(TileMatrix *matrix);
/*M�trix felszabad�t�sa*/

Tile* GetTileFromPosition(TileMatrix *matrix, Vector2 position);
/*Keres�s a m�trixban poz�ci� alapj�n*/

Tile* GetTileByID(TileMatrix *matrix, int id);
/*Keres�s a m�trixban azonos�t� szerint*/

Item* GetItemByName(char *name, int n, Item *itemList);
/*T�rgy megkeres�se az inventoryban*/

#endif
