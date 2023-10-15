#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "GUI.h"
#include "Graphics.h"

#define TICK 30


typedef struct Tile //Alap csempe struktúra ezekre épül a játék
{
    int id;
    char name[20];
    Vector2 positionToMouse; //A csempe pozíciója relatív az egérhez, ez kell hogy mozgatásnál ne ugorjon oda az egérhez hanem tartsa a relatív helyzetet
    Vector2 coordinates;
    Image img;
} Tile;

typedef struct Button
{
    Image img;
} Button;

typedef struct Windmill
{
    Tile base;
    bool opened;
    GUI_Panel menu;
} Windmill;
typedef struct Bakery
{
    Tile base;
    bool opened;
    GUI_Panel menu;
} Bakery;
typedef struct Brewery
{
    Tile base;
    bool opened;
    GUI_Panel menu;
} Brewery;

typedef struct BuildItem
{
    char name[20];
    int price;
    int level;
} BuildItem;

typedef struct Construction
{
    int time;
    BuildItem item;
} Construction;


//Irányítás
void OnLeftMouseDown(SDL_Event e);
void OnRightMouseDown(SDL_Event e);
void OnLeftMouseUp(SDL_Event e);
void OnRightMouseUp(SDL_Event e);
void OnMouseMove(SDL_Event e);

bool LeftMouseDown;

int mapSize = 2500;
Tile tiles[2500];
Vector2 origin = {-300,1800};

Button buttons[2];
int buttonCount = 0;

//Épületek
Vector2 millPosition ={21,19};
Vector2 bakeryPosition ={18,21};
Vector2 breweryPosition ={18,19};
Vector2 shopPosition ={20,22};
Vector2 warehousePosition ={23,22};

//Mértékek
int windowSizeX = 1000;
int windowSizeY = 700;
int baseSize = 200;

SDL_Window *window;
SDL_Renderer *renderer;

Windmill mill;
Bakery bakery;
Brewery brewery;
GUI_Panel buildPanel;
GUI_Panel plantPanel;
Image buildButton;
Image plantButton;

Image guis[50];

int money = 100;

bool BuildMode = false;
int actualBuildItem = 0;
BuildItem buildings[4];
int BuildItemCount = 3;
char Undestroyables[15][7] = {"Windmill","Bakery","Brewery","Shop","Warehouse","Ground",""};
Construction constructions[100];

TTF_Font *font;


Image moneyText;
Image timeText;
Vector2 moneyPos;


Image moneyBox;

Image timeBox;

Image guiElements[100];


void Init_Parameters()
{
    //Betűkészlet betöltése
    TTF_Init();
    font = TTF_OpenFont("system.fon", 20);

    //Az építhető épületek deklarálása
    buildings[0] = (BuildItem){"Field",100,1};
    buildings[1] = (BuildItem){"Beehive",1000,10};
    buildings[2] = (BuildItem){"Greenhouse",1000,10};
    buildings[3] = (BuildItem){"Ground",100,1};

    //Ablak léterhozása
    CreateWindow("FarmerRealm", windowSizeX, windowSizeY, &window, &renderer);
}

void Init_Map()
{
    FILE *file;

    file = fopen("map.txt", "w");


    int rowSize = sqrt(mapSize);

    for(int x = 0; x<rowSize;x++)
    {
        for(int y = 0; y<rowSize;y++)
        {
            char textureName[mapSize];

            Vector2 delta = (Vector2){(y*baseSize/2)-x*baseSize/2,(y*baseSize/4)+x*baseSize/4};
            delta = (Vector2){delta.x-origin.x, delta.y-origin.y};

            if(x==millPosition.x && y==millPosition.y)
            {
                CreateImage(renderer,"Images/Windmill.png",delta,(Vector2){baseSize,baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Windmill");
                mill.base = tiles[y+x*rowSize];
            }
            else if(x==bakeryPosition.x && y==bakeryPosition.y)
            {
                CreateImage(renderer,"Images/Bakery.png",delta,(Vector2){baseSize,baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Bakery");
            }
            else if(x==shopPosition.x && y==shopPosition.y)
            {
                CreateImage(renderer,"Images/Shop.png",delta,(Vector2){baseSize,baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Shop");
            }
            else if(x==warehousePosition.x && y==warehousePosition.y)
            {
                CreateImage(renderer,"Images/Warehouse.png",delta,(Vector2){baseSize,baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Warehouse");
            }
            else if(x==breweryPosition.x && y==breweryPosition.y)
            {
                CreateImage(renderer,"Images/Brewery.png",delta,(Vector2){baseSize,baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Brewery");
            }
            else if(rand()%2==0)
            {
                CreateImage(renderer,"Images/Ground.png",delta,(Vector2){baseSize,baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Ground");
            }
            else
            {
                CreateImage(renderer,"Images/Tree.png",delta,(Vector2){baseSize,baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Tree");
            }


            tiles[y+x*rowSize].id = y+x*rowSize;
            tiles[y+x*rowSize].coordinates = (Vector2){x,y};

            char text[30];
            sprintf(&text, "%d:%d %s\n",x,y,tiles[y+x*rowSize].name);

            fprintf(file,text );
        }

    }


    fclose(file);
}

void Init_GUI()
{

    moneyPos = GetUpRightCornerPosition((Vector2){0,0},(Vector2){windowSizeX,windowSizeY},(Vector2){300,100});

    CreateImage(renderer,"Images/GUI/MoneyBox.png",moneyPos,(Vector2){300,100}, &moneyBox);

    moneyPos.x = moneyPos.x+80;
    moneyPos.y = moneyPos.y+25;

    CreateImage(renderer,"Images/GUI/MoneyBox.png",(Vector2){0,0},(Vector2){300,100}, &timeBox);

    CreateImage(renderer,"Images/GUI/BuildButton.png",(Vector2){windowSizeX-100, windowSizeY-100},(Vector2){100,100}, &buildButton);
    CreateImage(renderer,"Images/GUI/PlantButton.png",(Vector2){windowSizeX-210, windowSizeY-100},(Vector2){100,100}, &plantButton);

    CreateImage(renderer,"Images/GUI/WidePanel.png",(Vector2){0, windowSizeY-220},(Vector2){windowSizeX,220}, &buildPanel.panelImage);
    CreateImage(renderer,"Images/GUI/Exit.png",(Vector2){windowSizeX-60, windowSizeY-220},(Vector2){50,50}, &buildPanel.children[buildPanel.childCount]);
    buildPanel.childCount++;
    CreateText("Build",(Color){80,52,30},(Vector2){150,50},(Vector2){20,windowSizeY-270},renderer,font, &buildPanel.children[buildPanel.childCount]);
    buildPanel.childCount++;

    for(int i = 0; i<BuildItemCount; i++)
    {

        CreateImage(renderer,"Images/GUI/Item.png",(Vector2){20+i*160, windowSizeY-200},(Vector2){150,180}, &buildPanel.children[buildPanel.childCount]);
        buildPanel.childCount++;
        char path[20];
        sprintf(&path,"Images/%s.png",buildings[i].name);

        CreateImage(renderer,path ,(Vector2){35+i*160, windowSizeY-220},(Vector2){120,120},&buildPanel.children[buildPanel.childCount]);
        buildPanel.childCount++;
        CreateText(buildings[i].name,(Color){80,52,30},(Vector2){90,20},(Vector2){50+i*160,windowSizeY-100},renderer,font, &buildPanel.children[buildPanel.childCount]);
        buildPanel.childCount++;

    }

    CreateImage(renderer,"Images/GUI/Bulldozer.png",(Vector2){windowSizeX-230, windowSizeY-200},(Vector2){150,180}, &buildPanel.children[buildPanel.childCount]);
    buildPanel.childCount++;

    CreateImage(renderer,"Images/GUI/WidePanel.png",(Vector2){0, windowSizeY-220},(Vector2){windowSizeX,220}, &plantPanel.panelImage);
    CreateImage(renderer,"Images/GUI/Exit.png",(Vector2){windowSizeX-60, windowSizeY-220},(Vector2){50,50}, &plantPanel.children[plantPanel.childCount]);
    plantPanel.childCount++;
    CreateText("Plant",(Color){80,52,30},(Vector2){150,50},(Vector2){20,windowSizeY-200},renderer,font, &plantPanel.children[plantPanel.childCount]);
    plantPanel.childCount++;

    CreateCraftPanel(renderer,"Windmill", (Vector2){windowSizeX,windowSizeY}, &mill.menu);
    CreateCraftPanel(renderer,"Bakery", (Vector2){windowSizeX,windowSizeY}, &bakery.menu);
    CreateCraftPanel(renderer,"Brewery", (Vector2){windowSizeX,windowSizeY}, &brewery.menu);

}

int main(int argc, char *argv[])
{

    Init_Parameters();
    Init_GUI();
    Init_Map();

    srand(time(0));

    int db = 0;


    SDL_SetRenderDrawColor(renderer, 3, 190, 252, 255);

    unsigned timer = SDL_GetTicks();

    SDL_Event event;
    while (event.type != SDL_QUIT)
    {

        while (SDL_PollEvent(&event))
        {
            switch(event.type)
                {
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == 1)
                        OnLeftMouseDown(event);
                    else if(event.button.button == 3)
                        OnRightMouseDown(event);
                    break;
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == 1)
                        OnLeftMouseUp(event);
                    else if(event.button.button == 3)
                        OnRightMouseUp(event);
                    break;
                case SDL_MOUSEMOTION:
                    OnMouseMove(event);
                    break;

            }
        }

        SDL_Delay(10);
        unsigned now = SDL_GetTicks();
        unsigned delta_time = now - timer;

        timer = now;

        int hour = timer/1000/60/60;
        int min = (timer/1000/60)-(hour*60);
        int sec = (timer/1000)-(hour*60*60)-(min*60);

        char intToString[20];
        sprintf(&intToString, "%02d:%02d:%02d", hour,min,sec);
        CreateText(intToString, (Color){80,52,30}, (Vector2){150,50}, (Vector2){80,25}, renderer, font, &timeText);

        sprintf(&intToString, "$%d", money);
        CreateText(intToString, (Color){80,52,30}, (Vector2){150,50}, moneyPos, renderer, font, &moneyText);

        //printf("%lf\n", (double)timer/1000/60);

        SDL_RenderClear(renderer);

        for(int i = 0; i<mapSize; i++)
        {
            //if(VectorLength(tiles[i].positionToMouse) < 1000)
            SDL_RenderCopy(renderer, tiles[i].img.texture, NULL, &tiles[i].img.destination);

            if(BuildMode && strcmp(buildings[actualBuildItem].name, "Ground") != 0)
            {
                if(BuildMode && strcmp(tiles[i].name,"Ground") != 0)
                {
                    SDL_SetTextureColorMod(tiles[i].img.texture,255,150,150);
                }
            }
            else if(BuildMode)
            {
                bool destroyable = true;
                int index = 0;

                while(strcmp(Undestroyables[index],"") != 0 && !destroyable)
                {
                    destroyable = strcmp(Undestroyables[index],tiles[i].name) != 0;
                    index++;
                }

                if(destroyable)
                    SDL_SetTextureColorMod(tiles[i].img.texture,255,150,150);
            }
        }

        for(int i = 0; i<buttonCount; i++)
            SDL_RenderCopy(renderer, buttons[i].img.texture, NULL, &buttons[i].img.destination);

        SDL_RenderCopy(renderer, moneyBox.texture, NULL, &moneyBox.destination);
        SDL_RenderCopy(renderer, moneyText.texture, NULL, &moneyText.destination);

        SDL_RenderCopy(renderer, timeBox.texture, NULL, &timeBox.destination);
        SDL_RenderCopy(renderer, timeText.texture, NULL, &timeText.destination);

        SDL_RenderCopy(renderer, buildButton.texture, NULL, &buildButton.destination);
        SDL_RenderCopy(renderer, plantButton.texture, NULL, &plantButton.destination);

        if(buildPanel.visible)
            RenderParent(renderer, buildPanel);

        if(plantPanel.visible)
            RenderParent(renderer, plantPanel);

        if(mill.opened)
        {
            RenderParent(renderer, mill.menu);
        }
        else if(bakery.opened)
        {
            RenderParent(renderer, bakery.menu);
        }
        else if(brewery.opened)
        {
            RenderParent(renderer, brewery.menu);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_Quit();

    return 0;
}

Tile lastMouseOver;
Vector2 lastMousePos;

void OnLeftMouseDown(SDL_Event e)
{
    LeftMouseDown = true;
    lastMousePos = (Vector2){e.motion.x,e.motion.y};

    for(int i = 0; i<mapSize;i++)
        tiles[i].positionToMouse = (Vector2){e.motion.x-tiles[i].img.destination.x,e.motion.y-tiles[i].img.destination.y};

    SDL_SetTextureColorMod(lastMouseOver.img.texture,200,200,200);
}
void OnRightMouseDown(SDL_Event e)
{

}
void OnLeftMouseUp(SDL_Event e)
{
    LeftMouseDown = false;

    //Kiszámítjuk az egérrel megtett út hosszát, hogy megállapítsuk, hogy click volt-e vagy vonszolás
    double moved = sqrt(pow(lastMousePos.x-e.motion.x,2)+pow(lastMousePos.y-e.motion.y,2));

    if(moved < 10)
    {
        if(strcmp(lastMouseOver.name,"Windmill")  == 0 && !mill.opened)
            mill.opened = true;
        else if(strcmp(lastMouseOver.name,"Bakery") == 0 && !bakery.opened)
            bakery.opened = true;
        else if(strcmp(lastMouseOver.name,"Brewery") == 0 && !brewery.opened)
            brewery.opened = true;



        for(int i =2; i<buildPanel.childCount ; i+= 3)
        {
            if(OverUI((Vector2){e.motion.x, e.motion.y}, buildPanel.children[i].destination) && buildPanel.visible)
            {
                printf("Building: ");
                BuildMode = true;
                actualBuildItem = (i-2)/3;
                printf("%s\n",buildings[actualBuildItem]);
            }
        }

        if(OverUI((Vector2){e.motion.x,e.motion.y}, mill.menu.children[0].destination) && mill.opened)
        {
            mill.opened = false;
        }

        if(OverUI((Vector2){e.motion.x,e.motion.y}, bakery.menu.children[0].destination) && bakery.opened)
        {
            bakery.opened = false;
        }

        if(OverUI((Vector2){e.motion.x,e.motion.y}, brewery.menu.children[0].destination) && brewery.opened)
        {
            brewery.opened = false;
        }

        if(OverUI((Vector2){e.motion.x,e.motion.y},  buildPanel.children[0].destination) && buildPanel.visible)
        {
            buildPanel.visible = false;
            BuildMode = false;
            for(int i = 0; i<mapSize; i++)
            {
                SDL_SetTextureColorMod(tiles[i].img.texture,255,255,255);
            }
        }
        if(OverUI((Vector2){e.motion.x,e.motion.y},  plantPanel.children[0].destination) && plantPanel.visible)
        {
            plantPanel.visible = false;
        }

        if(OverUI((Vector2){e.motion.x,e.motion.y},  buildButton.destination) && !buildPanel.visible)
        {
            buildPanel.visible = true;
        }
        if(OverUI((Vector2){e.motion.x,e.motion.y},  plantButton.destination) && !plantPanel.visible)
        {
            plantPanel.visible = true;
        }

        SDL_SetTextureColorMod(lastMouseOver.img.texture,220,220,220);
        printf("[%d](%d:%d) - %s\n",lastMouseOver.id,lastMouseOver.coordinates.x, lastMouseOver.coordinates.y, lastMouseOver.name);

        if(BuildMode && strcmp(lastMouseOver.name, "Ground") == 0)
        {
            printf("Created: Field\n");
            char path[30];
            sprintf(&path, "Images/%s.png", buildings[actualBuildItem].name);
            /*FILE *file = fopen("map.txt","r");

            char save[10000];

            fgets(save,10000,file);

            fclose(file);

            printf("%s",save);*/

            CreateImage(renderer,"Images/ConstructionSite.png",(Vector2){lastMouseOver.img.destination.x,lastMouseOver.img.destination.y},(Vector2){baseSize,baseSize}, &tiles[lastMouseOver.id].img);
            strcpy(tiles[lastMouseOver.id].name,buildings[actualBuildItem].name);
            BuildMode = false;

            for(int i = 0; i<mapSize; i++)
            {
                SDL_SetTextureColorMod(tiles[i].img.texture,255,255,255);
            }
        }


    }
    else
    {

    }
}
void OnRightMouseUp(SDL_Event e)
{

}
void OnMouseMove(SDL_Event e)
{
    //Ha bármilyen panel meg van nyitva akkor kilép
    if(bakery.opened || mill.opened || brewery.opened)
        return;


    //Térkép mozgatása
    for(int i = 0; i<mapSize; i++)
    {
        if(LeftMouseDown)
        {
            tiles[i].img.destination.x = (e.motion.x-tiles[i].positionToMouse.x);
            tiles[i].img.destination.y = (e.motion.y-tiles[i].positionToMouse.y);
        }
    }


    //Kép színének megváltoztatása ha rajta az egér
    bool finded = false;
    int index = 0;

    while(!finded && index < mapSize)
    {
        if(MouseOverImage(tiles[index].img.destination, (Vector2){e.motion.x,e.motion.y}, baseSize))
        {
            if(tiles[index].id != lastMouseOver.id)
            {
                SDL_SetTextureColorMod(tiles[index].img.texture,220,220,220);
                SDL_SetTextureColorMod(lastMouseOver.img.texture,255,255,255);
                lastMouseOver = tiles[index];
            }

            finded = true;
        }

        index++;
    }

}


