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
#include <string.h>

#include "Graphics.h"


typedef struct GUI_Panel //Alap GUI panel
{
    Image panelImage;
    Image children[30];

    int childCount;

    bool visible;
} GUI_Panel;

typedef struct ShopItem
{
    char name[50];
    int price;
    int level;
    int time;
    unsigned char ID;
} ShopItem;

typedef struct Canvas
{
    GUI_Panel buildPanel;
    GUI_Panel plantPanel;
    Image buildButton;
    Image plantButton;
    Image infoBox;
    Image moneyText;
    Image levelText;
} Canvas;

void CreateText(char text[], Color color, int sizeOfText, Vector2 position, SDL_Renderer* renderer, TTF_Font *font, Image *img);
/*Szöveg kirajzolása*/

void RenderParent(SDL_Renderer* renderer, GUI_Panel parent);
/*Összegyüjti a gyermek objektumokat és megjeleníti őket*/

void RenderShopPanel(SDL_Renderer* renderer, GUI_Panel *panel,ShopItem *items, int childcount ,int money, int level);
/*Megjeleníti a vásárló felületeket és megváltoztatja a hozzáférhetés szerint a színeket*/

Vector2 GetUpRightCornerPosition(Vector2 parentPos, Vector2 parentSize, Vector2 objSize);
Vector2 GetUpLeftCornerPosition(Vector2 parentPos, Vector2 parentSize, Vector2 objSize);

bool OverUI(Vector2 mousePos, SDL_Rect obj);
/*Ellenőrzi hogy a megadott koordináta a megadott interfész elem felett van-e*/

void CreateCraftPanel(SDL_Renderer* renderer,char *title, Vector2 windowSize, GUI_Panel *panel);
/*Létrehozza a "barkácsolás" menü elemeket*/

int GetTextLength(char *text);
/*Visszatér a char[] paraméter hosszával (szükséges a szöveg méretének meghatározásához)*/

void ShowAnimatedGUI(SDL_Renderer* renderer, GUI_Panel *panel, int windowSizeY, ShopItem *items, int childcount ,int money, int level);
/*Kiszámolja és legenerálja az eltűnés/előjövetel animációt*/

void CheckShopItems(GUI_Panel *parent, ShopItem *items, int childcount ,int money, int level);

void FormatTime(int t, char *out);

void RenderCanvas(Canvas *canvas, SDL_Renderer *renderer);
#endif // GUI_H
