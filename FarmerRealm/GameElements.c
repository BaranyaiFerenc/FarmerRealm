#include "GameElements.h"

Tile** CreateMatrix(int xSize, int ySize)
{
    Tile** matrix = (Tile**)malloc(ySize*sizeof(Tile));

    for(int y = 0; y<ySize; y++)
    {
        matrix[y] = (Tile*)malloc(xSize*sizeof(Tile));
    }

    return matrix;
}

Tile* GetTileFromPosition(TileMatrix *matrix, Vector2 position)
{



    for(int x = 0; x<matrix->xSize; x++)
    {
        for(int y=0; y<matrix->ySize; y++)
        {
            if(MouseOverImage(matrix->matrix[x][y].img.destination, position, 200))
            {
                return &matrix->matrix[x][y];
            }
        }
    }
}

Tile* GetTileByID(TileMatrix *matrix, int id)
{
    int row = id/matrix->ySize;
    int col = id-row*matrix->ySize;
    return &matrix->matrix[row][col];
}


void FreeMatrix(TileMatrix *matrix)
{
    for(int i = 0; i<matrix->ySize; i++)
    {
        free(matrix->matrix[i]);
    }

    free(matrix->matrix);
}
