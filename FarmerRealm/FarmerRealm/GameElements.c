#include "GameElements.h"

void AddElementToMatrix(TileMatrix *tileList, Tile tile, int r, int c)
{
    //Megállapítjuk hogy létre kell-e hozni új sort a mátrixban?
    int rowCount = tileList ->row_size > r+1 ? tileList->row_size:r+1;
    int columnCount = tileList ->column_size > c+1 ? tileList->column_size:c+1;

    Tile *newMatrix = (Tile*)malloc(sizeof(Tile)*rowCount*columnCount);

    for(int y = 0; y<tileList->row_size; y++)
    {
        for(int x = 0; x<tileList->column_size; x++)
        {
            newMatrix[y*x] = tileList->matrix[x*y];
        }
    }
    //printf("Matrix created with size of %dx%d\n",rowCount,columnCount);
    newMatrix[r*rowCount+c] = tile;

    free(tileList->matrix);
    tileList->matrix = newMatrix;
    tileList->column_size=columnCount;
    tileList->row_size = rowCount;
}
