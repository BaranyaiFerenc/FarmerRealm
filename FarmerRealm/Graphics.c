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


bool MouseOverImage(SDL_Rect img, Vector2 mousePos, int sizeOfBase)//Ellen�rzi hogy a k�p felett van az eg�r
{
    int mouseToOrigoX = (img.x+(img.w/2))-mousePos.x; //Az eg�r kordin�t�ja relat�v a k�p helyzet�hez
    int UpperCord = img.y-UpperY(mouseToOrigoX,sizeOfBase)+(sizeOfBase*1.0/4.0); //Legmagasabb pont ami m�g az alapon van az aktu�lis x �rt�khez k�pest
    int LowerCord = img.y-LowerY(mouseToOrigoX,sizeOfBase)+(sizeOfBase*3.0/4.0); //Legalacsonyabb pont ami m�g az alapon van az aktu�lis x �rt�khez k�pest

    return LowerCord > mousePos.y && UpperCord < mousePos.y;
}
//Abszol�t �rt�k f�ggv�ny mely megadja az izometrikus alap fels� hat�rait relat�van az x kordin�t�hoz
int UpperY(int x,int sizeOfBase)
{
    return -((abs(x)/2)+(sizeOfBase/4));
}

//Abszol�t �rt�k f�ggv�ny mely megadja az izometrikus alap als� hat�rait relat�van az x kordin�t�hoz
int LowerY(int x,int sizeOfBase)
{
    return (abs(x)/2)-(sizeOfBase/4);
}


int VectorLength(Vector2 v)
{
    return sqrt(pow(v.x,2)+pow(v.y,2));
}
