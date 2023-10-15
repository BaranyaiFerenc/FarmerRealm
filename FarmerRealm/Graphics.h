#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <math.h>

typedef struct Vector2 //Kordin�ta strukt�ra
{
    int x,y;
} Vector2;


typedef struct Color //Sz�n
{
    int r,g,b;
} Color;

typedef struct Image //Alap k�p strukt�ra az �tl�that�bb �s egyszer�bb k�d�rt
{
    SDL_Texture* texture;
    SDL_Rect destination;
} Image;

typedef enum ImageType {game, gui} ImageType;

/*Ablak l�trehoz�sa*/
void CreateWindow(char const *text, int width, int height, SDL_Window **pwindow, SDL_Renderer **prenderer);

/*K�pek l�trehoz�sa*/
void CreateImage(SDL_Renderer* renderer, char const path[], Vector2 positionToPlace, Vector2 size, Image *img);

bool MouseOverImage(SDL_Rect img, Vector2 mousePos, int sizeOfBase);
int UpperY(int x,int sizeOfBase);
int LowerY(int x,int sizeOfBase);

int VectorLength(Vector2 v);

#endif // GRAPHICS_H
