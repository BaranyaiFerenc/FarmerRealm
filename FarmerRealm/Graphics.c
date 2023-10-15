#include "Graphics.h"

void CreateWindow(char const *text, int width, int height, SDL_Window **pwindow, SDL_Renderer **prenderer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow(text, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (window == NULL)
    {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL)
    {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }

    SDL_SetWindowIcon(window,IMG_Load("Images/Windmill.png"));

    SDL_RenderClear(renderer);

    *pwindow = window;
    *prenderer = renderer;
}

void CreateImage(SDL_Renderer* renderer, char const path[], Vector2 positionToPlace, Vector2 size, Image *img)
{
    SDL_Surface* surface = IMG_Load(path);
    img -> texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    SDL_QueryTexture(img -> texture, NULL, NULL, &img -> destination.w, &img -> destination.h);

    img -> destination.w = size.x;
    img -> destination.h = size.y;

    img -> destination.x = positionToPlace.x;
    img -> destination.y = positionToPlace.y;
}


bool MouseOverImage(SDL_Rect img, Vector2 mousePos, int sizeOfBase)//Ellenõrzi hogy a kép felett van az egér
{
    int mouseToOrigoX = (img.x+(img.w/2))-mousePos.x; //Az egér kordinátája relatív a kép helyzetéhez
    int UpperCord = img.y-UpperY(mouseToOrigoX,sizeOfBase)+(sizeOfBase*1.0/4.0); //Legmagasabb pont ami még az alapon van az aktuális x értékhez képest
    int LowerCord = img.y-LowerY(mouseToOrigoX,sizeOfBase)+(sizeOfBase*3.0/4.0); //Legalacsonyabb pont ami még az alapon van az aktuális x értékhez képest

    return LowerCord > mousePos.y && UpperCord < mousePos.y;
}
//Abszolút érték függvény mely megadja az izometrikus alap felsõ határait relatívan az x kordinátához
int UpperY(int x,int sizeOfBase)
{
    return -((abs(x)/2)+(sizeOfBase/4));
}

//Abszolút érték függvény mely megadja az izometrikus alap alsó határait relatívan az x kordinátához
int LowerY(int x,int sizeOfBase)
{
    return (abs(x)/2)-(sizeOfBase/4);
}


int VectorLength(Vector2 v)
{
    return sqrt(pow(v.x,2)+pow(v.y,2));
}
