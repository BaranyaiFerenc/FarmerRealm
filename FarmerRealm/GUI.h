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
#include "GameElements.h"


typedef struct GUI_Panel //Alap GUI panel
{
    Image panelImage;
    Image children[70];

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


typedef struct ShopData
{
    int actualItem;
    ShopItem Items[6];
    unsigned short int ChildCount;
    unsigned short int ItemCount;
} ShopData;

typedef struct Canvas
{
    GUI_Panel buildPanel;
    GUI_Panel plantPanel;
    GUI_Panel warehousePanel;
    Image buildButton;
    Image plantButton;
    Image infoBox;
    Image moneyText;
    Image levelText;
    Image levelProgression;
} Canvas;

void CreateText(char text[], Color color, int sizeOfText, Vector2 position, SDL_Renderer* renderer, TTF_Font *font, Image *img);
/*Szöveg kirajzolása*/

void RenderParent(SDL_Renderer* renderer, GUI_Panel parent);
/*Összegyüjti a gyermek objektumokat és megjeleníti õket*/

void RenderShopPanel(SDL_Renderer* renderer, GUI_Panel *panel,ShopItem *items, int childcount ,int money, int level);
/*Megjeleníti a vásárló felületeket és megváltoztatja a hozzáférhetés szerint a színeket*/

Vector2 GetUpRightCornerPosition(Vector2 parentPos, Vector2 parentSize, Vector2 objSize);
Vector2 GetUpLeftCornerPosition(Vector2 parentPos, Vector2 parentSize, Vector2 objSize);

bool OverUI(Vector2 mousePos, SDL_Rect obj);
/*Ellenõrzi hogy a megadott koordináta a megadott interfész elem felett van-e*/

void CreateCraftPanel(SDL_Renderer* renderer,char *title, Vector2 windowSize, ArrayData *RecipesData, RecipeType type, Item *inventory, GUI_Panel *panel);
/*Létrehozza a "barkácsolás" menü elemeket*/

int GetTextLength(char *text);
/*Visszatér a char[] paraméter hosszával (szükséges a szöveg méretének meghatározásához)*/

void ShowAnimatedGUI(SDL_Renderer* renderer, GUI_Panel *panel, int windowSizeY, ShopItem *items, int childcount ,int money, int level);
/*Kiszámolja és legenerálja az eltûnés/elõjövetel animációt*/

void FormatTime(int t, char *out);

void RenderCanvas(Canvas *canvas, SDL_Renderer *renderer);
/*Az alap GUI megjelenítése*/

void OpenWarehouse(Canvas *canvas, SDL_Renderer *renderer, ArrayData *InvData,TTF_Font *font,int *ids);
/*A raktár listájának létrehozása*/

void SetParentAlpha(GUI_Panel *parent, int a);

#endif // GUI_H
