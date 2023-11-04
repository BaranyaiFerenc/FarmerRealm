#ifndef GAMEELEMENTS_H
#define GAMEELEMENTS_H

#include "Graphics.h"

typedef struct Tile //Alap csempe struktúra ezekre épül a játék
{
    unsigned short int id;
    char name[20];
    Vector2 positionToMouse; //A csempe pozíciója relatív az egérhez, ez kell hogy mozgatásnál ne ugorjon oda az egérhez hanem tartsa a relatív helyzetet
    Vector2 coordinates;
    Image img;
    Image additionalImage;
    bool additional;
} Tile;

typedef struct TileMatrix
{
    Tile *matrix;
    int row_size;
    int column_size;
} TileMatrix;


void AddElementToMatrix(TileMatrix *matrix, Tile tile, int r, int c);

#endif
