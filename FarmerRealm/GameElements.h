#ifndef GAMEELEMENTS_H
#define GAMEELEMENTS_H

#include "Graphics.h"

typedef struct Tile //Alap csempe strukt�ra ezekre �p�l a j�t�k
{
    unsigned short int id;
    char name[20];
    Vector2 positionToMouse; //A csempe poz�ci�ja relat�v az eg�rhez, ez kell hogy mozgat�sn�l ne ugorjon oda az eg�rhez hanem tartsa a relat�v helyzetet
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
