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
/*Sz�veg kirajzol�sa*/

void RenderParent(SDL_Renderer* renderer, GUI_Panel parent);
/*�sszegy�jti a gyermek objektumokat �s megjelen�ti �ket*/

void RenderShopPanel(SDL_Renderer* renderer, GUI_Panel *panel,ShopItem *items, int childcount ,int money, int level);
/*Megjelen�ti a v�s�rl� fel�leteket �s megv�ltoztatja a hozz�f�rhet�s szerint a sz�neket*/

Vector2 GetUpRightCornerPosition(Vector2 parentPos, Vector2 parentSize, Vector2 objSize);
Vector2 GetUpLeftCornerPosition(Vector2 parentPos, Vector2 parentSize, Vector2 objSize);

bool OverUI(Vector2 mousePos, SDL_Rect obj);
/*Ellen�rzi hogy a megadott koordin�ta a megadott interf�sz elem felett van-e*/

void CreateCraftPanel(SDL_Renderer* renderer,char *title, Vector2 windowSize, ArrayData *RecipesData, RecipeType type, Item *inventory, GUI_Panel *panel);
/*L�trehozza a "bark�csol�s" men� elemeket*/

int GetTextLength(char *text);
/*Visszat�r a char[] param�ter hossz�val (sz�ks�ges a sz�veg m�ret�nek meghat�roz�s�hoz)*/

void ShowAnimatedGUI(SDL_Renderer* renderer, GUI_Panel *panel, int windowSizeY, ShopItem *items, int childcount ,int money, int level);
/*Kisz�molja �s legener�lja az elt�n�s/el�j�vetel anim�ci�t*/

void FormatTime(int t, char *out);

void RenderCanvas(Canvas *canvas, SDL_Renderer *renderer);
/*Az alap GUI megjelen�t�se*/

void OpenWarehouse(Canvas *canvas, SDL_Renderer *renderer, ArrayData *InvData,TTF_Font *font,int *ids);
/*A rakt�r list�j�nak l�trehoz�sa*/

void SetParentAlpha(GUI_Panel *parent, int a);

#endif // GUI_H
