#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <math.h>

typedef struct Vector2 //Kordináta struktúra
{
    int x,y;
} Vector2;


typedef struct Color //Szín
{
    int r,g,b;
} Color;

typedef struct Image //Alap kép struktúra az átláthatóbb és egyszerûbb kódért
{
    SDL_Texture* texture;
    SDL_Rect destination;
} Image;

typedef struct Source
{
    SDL_Texture* texture;
    char path[100];
} Source;

typedef enum ImageType {game, gui} ImageType;

SDL_Texture* GetSource(Source *sourceList, char *path, SDL_Renderer *renderer);
void LoadSource(Source *sourceList, char *path, SDL_Renderer *renderer);

/*Ablak létrehozása*/
void CreateWindow(char const *text, int width, int height, SDL_Window **pwindow, SDL_Renderer **prenderer);

/*Képek létrehozása*/
void CreateImage(SDL_Renderer* renderer,  char *path, Vector2 positionToPlace, Vector2 size, Image *img);

bool MouseOverImage(SDL_Rect img, Vector2 mousePos, int sizeOfBase);
int UpperY(int x,int sizeOfBase);
int LowerY(int x,int sizeOfBase);

int VectorLength(Vector2 v);

#endif // GRAPHICS_H
