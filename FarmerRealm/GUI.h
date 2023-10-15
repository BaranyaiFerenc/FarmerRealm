#ifndef GUI_H
#define GUI_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <math.h>
#include <stdbool.h>

#include "Graphics.h"

typedef struct GUI_Panel //Alap GUI panel
{
    Image panelImage;
    Image children[20];

    int childCount;

    bool visible;
} GUI_Panel;

void CreateText(char const text[], Color color, Vector2 sizeOfText, Vector2 position, SDL_Renderer* renderer, TTF_Font *font, Image *img);
/*Szöveg kirajzolása*/

void RenderParent(SDL_Renderer* renderer, GUI_Panel parent);
/*Összegyüjti a gyermek objektumokat és megjeleníti őket*/

Vector2 GetUpRightCornerPosition(Vector2 parentPos, Vector2 parentSize, Vector2 objSize);
Vector2 GetUpLeftCornerPosition(Vector2 parentPos, Vector2 parentSize, Vector2 objSize);

bool OverUI(Vector2 mousePos, SDL_Rect obj);
/*Ellenőrzi hogy a megadott koordináta a megadott interfész elem felett van-e*/

void CreateCraftPanel(SDL_Renderer* renderer,char const title[], Vector2 windowSize, GUI_Panel *panel);
/*Létrehozza a "barkácsolás" menü elemeket*/

int GetTextLength(char text[]);
/*Visszatér a char[] paraméter hosszával (szükséges a szöveg méretének meghatározásához)*/
#endif // GUI_H
