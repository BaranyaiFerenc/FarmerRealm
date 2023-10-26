#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "GUI.h"
#include "Graphics.h"
#include "Filemanager.h"
#include "Inventory.h"

#define TICK 30


typedef struct Tile //Alap csempe struktúra ezekre épül a játék
{
    unsigned short int id;
    char name[20];
    Vector2 positionToMouse; //A csempe pozíciója relatív az egérhez, ez kell hogy mozgatásnál ne ugorjon oda az egérhez hanem tartsa a relatív helyzetet
    Vector2 coordinates;
    Image img;
} Tile;

typedef struct Button
{
    Image img;
} Button;


typedef struct Process
{
    int tileID;
    double time;

    char path[100];
    int misc;
} Process;

typedef struct Crafter
{
    char name[100];
    Tile *base;
    bool opened;
    GUI_Panel menu;
    Process ActiveProcess[3];
} Crafter;


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
int windowSizeX = 1100;
int windowSizeY = 750;
int baseSize = 200;

SDL_Window *window;
SDL_Renderer *renderer;

Crafter crafters[3];

GUI_Panel buildPanel;
GUI_Panel plantPanel;
Image buildButton;
Image plantButton;

int PanelSpeed = 30;

void CloseBuildPanel();
void ClosePlantPanel();

Image guis[50];

int money = 10000;
int level = 1;

bool BuildMode = false;
int actualBuildItem = 0;
ShopItem Buildings[5];
unsigned short int BuildChildCount = 6;
unsigned short int BuildItemCount = 3;
Image BuildLockImages[3];

char Undestroyables[15][7] = {"Windmill","Bakery","Brewery","Shop","Warehouse","Ground",""};

bool PlantMode = false;
int actualPlantItem = 0;
ShopItem Plants[4];
unsigned short int PlantChildCount = 6;
unsigned short int PlantItemCount = 4;
Image PlantLockImages[3];

Process BuildProcesses[100];
Process PlantProcesses[100];
Process CraftProcesses[9];

void AddProcess(int id,unsigned char type, int t, char path[], int m);

TTF_Font *font;


Image moneyText;
Image levelText;
Image timeText;
Vector2 moneyPos;


Image moneyBox;

Image timeBox;

Image guiElements[100];

void ClearColors();
void SetBuildColors();
void SetPlantColors();


Tile *LastTile;
Vector2 lastMousePos;


Slot inventory[InventorySize];
Item AllItem[InventorySize];

unsigned timer;
unsigned plusTime;

void Init_Parameters()
{
    //Betűkészlet betöltése
    TTF_Init();
    font = TTF_OpenFont("system.fon", 20);

    //Inventory
    AllItem[0] = (Item){.Name = "Wheat", .IconPath = "Images/Icon/Wheat.png", .Price = 200};
    AllItem[1] = (Item){.Name = "Corn", .IconPath = "Images/Icon/Corn.png", .Price = 200};
    AllItem[2] = (Item){.Name = "Tomato", .IconPath = "Images/Icon/Tomato.png", .Price = 200};
    AllItem[3] = (Item){.Name = "Potato", .IconPath = "Images/Icon/Potato.png", .Price = 200};

    //Az építhető épületek deklarálása
    Buildings[0] = (ShopItem){"Field",100,1,10,21};
    Buildings[1] = (ShopItem){"Beehive",1000,5,60,22};
    Buildings[2] = (ShopItem){"Greenhouse",5000,10,300,23};
    Buildings[3] = (ShopItem){"Ground",100,1,60,10}; //Megsemmisítés gombhoz
    Buildings[4] = (ShopItem){"ConstructionSite",0,1,60,1}; //Építés alatt álló épületek

    //Ültethető növények deklarálása
    Plants[0] = (ShopItem){"Wheat",1,1,30,101};
    Plants[1] = (ShopItem){"Potato",2,2,60,102};
    Plants[2] = (ShopItem){"Corn",5,3,120,103};
    Plants[3] = (ShopItem){"Tomato",10,5,240,104};

    //A "barkács" épületek deklarálása
    crafters[0] = (Crafter){"Windmill"};
    crafters[1] = (Crafter){"Brewery"};
    crafters[2] = (Crafter){"Bakery"};

    //Ablak léterhozása
    CreateWindow("FarmerRealm", windowSizeX, windowSizeY, &window, &renderer);

    for(int i = 0; i<100;i++)
        BuildProcesses[i].tileID = -1;

    LastTile = &tiles[0];
}

void Init_Map()
{
    FILE *file;

    file = fopen("save.bin", "r");
    int rowSize = sqrt(mapSize);
    if(file != NULL)
    {
        fclose(file);
        unsigned long long int saves[2500];

        GetSave("save.bin",&saves);
        plusTime = (saves[0] << 32 >> 32)*1000;
        money = (saves[0] << 16 >> 48);
        printf("Last time: %u\n",plusTime);
        printf("Last money: %d\n",money);
        printf("Save found\n");

        for(int x = 0; x<rowSize;x++)
        {
            for(int y = 0; y<rowSize;y++)
            {
                char textureName[mapSize];

                Vector2 delta = (Vector2){(y*baseSize/2)-x*baseSize/2,(y*baseSize/4)+x*baseSize/4};
                delta = (Vector2){delta.x-origin.x, delta.y-origin.y};

                int index = y+x*rowSize;
                int type = saves[index+1] << 12 >> 56;

                if(x==millPosition.x && y==millPosition.y)
                {
                    CreateImage(renderer,"Images/Windmill.png",delta,(Vector2){baseSize,baseSize}, &tiles[index].img);
                    strcpy(tiles[index].name,"Windmill");
                    crafters[0].base = &tiles[index];
                }
                else if(x==bakeryPosition.x && y==bakeryPosition.y)
                {
                    CreateImage(renderer,"Images/Bakery.png",delta,(Vector2){baseSize,baseSize}, &tiles[index].img);
                    strcpy(tiles[index].name,"Bakery");
                    crafters[2].base = &tiles[index];
                }
                else if(x==breweryPosition.x && y==breweryPosition.y)
                {
                    CreateImage(renderer,"Images/Brewery.png",delta,(Vector2){baseSize,baseSize}, &tiles[index].img);
                    strcpy(tiles[index].name,"Brewery");
                    crafters[1].base = &tiles[index];
                }
                else if(x==warehousePosition.x && y==warehousePosition.y)
                {
                    CreateImage(renderer,"Images/Warehouse.png",delta,(Vector2){baseSize,baseSize}, &tiles[index].img);
                    strcpy(tiles[index].name,"Warehouse");
                }
                else
                {
                    if(type == 11)
                    {
                        CreateImage(renderer,"Images/Tree.png",delta,(Vector2){baseSize,baseSize}, &tiles[index].img);
                        strcpy(tiles[index].name,"Tree");
                    }
                    else if(type == 10)
                    {
                        CreateImage(renderer,"Images/Ground.png",delta,(Vector2){baseSize,baseSize}, &tiles[index].img);
                        strcpy(tiles[index].name,"Ground");
                    }
                    else if(type == 1)
                    {
                        CreateImage(renderer,"Images/ConstructionSite.png",delta,(Vector2){baseSize,baseSize}, &tiles[index].img);
                        strcpy(tiles[index].name,"ConstructionSite");
                        unsigned long long int timeToDone = saves[index] << 20 >> 28;

                        //WriteOutBin(timeToDone);

                        unsigned char buildingType = saves[index] << 56 >> 56;

                        //WriteOutBin(buildingType);

                        printf("%lu sec is when bulding with type: %d is done\n", timeToDone, buildingType);
                    }
                    else
                    {
                        for(int i = 0; i<BuildItemCount;i++)
                        {
                            if(Buildings[i].BuildingID == type)
                            {
                                char path[30];
                                sprintf(&path,"Images/%s.png",Buildings[i].name);
                                CreateImage(renderer,path,delta,(Vector2){baseSize,baseSize}, &tiles[index].img);
                                strcpy(tiles[index].name,Buildings[i].name);
                            }
                        }
                    }


                }


                tiles[index].id = index;
                tiles[index].coordinates = (Vector2){x,y};
            }
        }
        return;
    }


    file = fopen("save.bin", "wb");

    unsigned long long int stats = 0;
    stats += 1;
    stats <<= 16;

    stats += 1000;
    stats <<= 32;

    fwrite(&stats, sizeof(stats), 1, file);

    for(int x = 0; x<rowSize;x++)
    {
        for(int y = 0; y<rowSize;y++)
        {
            char textureName[mapSize];

            Vector2 delta = (Vector2){(y*baseSize/2)-x*baseSize/2,(y*baseSize/4)+x*baseSize/4};
            delta = (Vector2){delta.x-origin.x, delta.y-origin.y};

            unsigned char type = 0;

            if(x==millPosition.x && y==millPosition.y)
            {
                CreateImage(renderer,"Images/Windmill.png",delta,(Vector2){baseSize,baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Windmill");
                crafters[0].base = &tiles[y+x*rowSize];

                type = 0;
            }
            else if(x==bakeryPosition.x && y==bakeryPosition.y)
            {
                CreateImage(renderer,"Images/Bakery.png",delta,(Vector2){baseSize,baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Bakery");
                crafters[2].base = &tiles[y+x*rowSize];

                type = 0;
            }
            else if(x==breweryPosition.x && y==breweryPosition.y)
            {
                CreateImage(renderer,"Images/Brewery.png",delta,(Vector2){baseSize,baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Brewery");
                crafters[1].base = &tiles[y+x*rowSize];

                type = 0;
            }
            else if(x==warehousePosition.x && y==warehousePosition.y)
            {
                CreateImage(renderer,"Images/Warehouse.png",delta,(Vector2){baseSize,baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Warehouse");

                type = 0;
            }
            else if(rand()%2==0)
            {
                CreateImage(renderer,"Images/Ground.png",delta,(Vector2){baseSize,baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Ground");

                type = 10;
            }
            else
            {
                CreateImage(renderer,"Images/Tree.png",delta,(Vector2){baseSize,baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Tree");

                type = 11;
            }


            tiles[y+x*rowSize].id = y+x*rowSize;
            tiles[y+x*rowSize].coordinates = (Vector2){x,y};

            char text[30];

            unsigned long long int log = GetBinary(y+x*rowSize, type, 0);

            fwrite(&log, sizeof(log), 1, file);
        }

    }

    fclose(file);
}

void Init_GUI()
{

    moneyPos = GetUpRightCornerPosition((Vector2){0,0},(Vector2){windowSizeX,windowSizeY},(Vector2){300,100});

    CreateImage(renderer,"Images/GUI/MoneyBox.png",moneyPos,(Vector2){300,126}, &moneyBox);

    moneyPos.x = moneyPos.x+80;
    moneyPos.y = moneyPos.y+25;

    CreateImage(renderer,"Images/GUI/MoneyBox.png",(Vector2){0,0},(Vector2){300,126}, &timeBox);

    CreateImage(renderer,"Images/GUI/BuildButton.png",(Vector2){windowSizeX-100, windowSizeY-100},(Vector2){100,100}, &buildButton);
    CreateImage(renderer,"Images/GUI/PlantButton.png",(Vector2){windowSizeX-210, windowSizeY-100},(Vector2){100,100}, &plantButton);

    //Építő menü létrehozása
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
        sprintf(&path,"Images/%s.png",Buildings[i].name);

        CreateImage(renderer,path ,(Vector2){35+i*160, windowSizeY-220},(Vector2){120,120},&buildPanel.children[buildPanel.childCount]);
        buildPanel.childCount++;
        CreateImage(renderer,"Images/GUI/Lock.png" ,(Vector2){40+i*160, windowSizeY-200},(Vector2){110,110},&buildPanel.children[buildPanel.childCount]);
        buildPanel.childCount++;

        CreateText(Buildings[i].name,(Color){80,52,30},(Vector2){90,20},(Vector2){50+i*160,windowSizeY-100},renderer,font, &buildPanel.children[buildPanel.childCount]);
        buildPanel.childCount++;
        char additionalText[10];
        sprintf(&additionalText,"$%d",Buildings[i].price);
        CreateText(additionalText,(Color){80,52,30},(Vector2){90,20},(Vector2){50+i*160,windowSizeY-60},renderer,font, &buildPanel.children[buildPanel.childCount]);
        buildPanel.childCount++;
        sprintf(&additionalText,"Level %d",Buildings[i].level);
        CreateText(additionalText,(Color){80,52,30},(Vector2){60,20},(Vector2){65+i*160,windowSizeY-80},renderer,font, &buildPanel.children[buildPanel.childCount]);
        buildPanel.childCount++;

    }

    CreateImage(renderer,"Images/GUI/Bulldozer.png",(Vector2){windowSizeX-230, windowSizeY-200},(Vector2){150,180}, &buildPanel.children[buildPanel.childCount]);
    buildPanel.childCount++;


    //Ültető menü létrehozása
    CreateImage(renderer,"Images/GUI/WidePanel.png",(Vector2){0, windowSizeY-220},(Vector2){windowSizeX,220}, &plantPanel.panelImage);
    CreateImage(renderer,"Images/GUI/Exit.png",(Vector2){windowSizeX-60, windowSizeY-220},(Vector2){50,50}, &plantPanel.children[plantPanel.childCount]);
    plantPanel.childCount++;
    CreateText("Plant",(Color){80,52,30},(Vector2){150,50},(Vector2){20,windowSizeY-270},renderer,font, &plantPanel.children[plantPanel.childCount]);
    plantPanel.childCount++;

    for(int i = 0; i<PlantItemCount; i++)
    {
        CreateImage(renderer,"Images/GUI/Item.png",(Vector2){20+i*160, windowSizeY-200},(Vector2){150,180}, &plantPanel.children[plantPanel.childCount]);
        plantPanel.childCount++;
        char path[20];
        sprintf(&path,"Images/Icons/%sseed.png",Plants[i].name,"seed");

        CreateImage(renderer,path ,(Vector2){45+i*160, windowSizeY-190},(Vector2){100,100},&plantPanel.children[plantPanel.childCount]);
        plantPanel.childCount++;
        CreateImage(renderer,"Images/GUI/Lock.png" ,(Vector2){40+i*160, windowSizeY-200},(Vector2){110,110},&plantPanel.children[plantPanel.childCount]);
        plantPanel.childCount++;
        CreateText(Plants[i].name,(Color){80,52,30},(Vector2){90,20},(Vector2){50+i*160,windowSizeY-100},renderer,font, &plantPanel.children[plantPanel.childCount]);
        plantPanel.childCount++;
        char additionalText[10];
        sprintf(&additionalText,"$%d",Plants[i].price);
        CreateText(additionalText,(Color){80,52,30},(Vector2){40,20},(Vector2){70+i*160,windowSizeY-60},renderer,font, &plantPanel.children[plantPanel.childCount]);
        plantPanel.childCount++;
        sprintf(&additionalText,"Level %d",Plants[i].level);
        CreateText(additionalText,(Color){80,52,30},(Vector2){60,20},(Vector2){65+i*160,windowSizeY-80},renderer,font, &plantPanel.children[plantPanel.childCount]);
        plantPanel.childCount++;

    }


    //A "barkács" épületekhez tartozó menü létrehozása
    CreateCraftPanel(renderer,"Windmill", (Vector2){windowSizeX,windowSizeY}, &crafters[0].menu);
    CreateCraftPanel(renderer,"Brewery", (Vector2){windowSizeX,windowSizeY}, &crafters[1].menu);
    CreateCraftPanel(renderer,"Bakery", (Vector2){windowSizeX,windowSizeY}, &crafters[2].menu);

}

int main(int argc, char *argv[])
{
    Init_Parameters();
    Init_GUI();
    Init_Map();

    SDL_SetRenderDrawColor(renderer, 3, 190, 252, 255);

    timer = SDL_GetTicks();

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
        unsigned now;
        unsigned delta_time;

        if(plusTime > 0)
        {
            now = plusTime;
            timer = plusTime;
            plusTime = 0;
        }
        else
        {
            now = SDL_GetTicks();
            delta_time = now - timer;
            timer = now;
        }




        //int hour, minute,sec;


        //FormatTime(timer, &hour,&minute,&sec);

        for(int i = 0; i<100;i++)
        {
            if(BuildProcesses[i].tileID != -1)
            {
                BuildProcesses[i].time -= delta_time;

                if(BuildProcesses[i].time <= 0)
                {
                    printf("Process %s done with ",BuildProcesses[i].path);
                    printf("type: %d saved\n",BuildProcesses[i].misc);
                    SetSave(BuildProcesses[i].tileID,BuildProcesses[i].misc,0);
                    CreateImage(renderer,BuildProcesses[i].path,(Vector2){tiles[BuildProcesses[i].tileID].img.destination.x,tiles[BuildProcesses[i].tileID].img.destination.y},(Vector2){baseSize,baseSize}, &tiles[BuildProcesses[i].tileID].img);
                    BuildProcesses[i].tileID = -1;
                }
            }


        }

        char intToString[20];
        FormatTime(timer,&intToString);
        CreateText(intToString, (Color){80,52,30}, (Vector2){150,50}, (Vector2){80,25}, renderer, font, &timeText);

        sprintf(&intToString, "$%d", money);
        CreateText(intToString, (Color){80,52,30}, (Vector2){150,50}, moneyPos, renderer, font, &moneyText);

        sprintf(&intToString, "Level %d", level);
        CreateText(intToString, (Color){80,52,30}, (Vector2){75,30}, (Vector2){moneyPos.x+35,moneyPos.y+60}, renderer, font, &levelText);

        //printf("%lf\n", (double)timer/1000/60);

        SDL_RenderClear(renderer);


        if(BuildMode)
            SetBuildColors();

        if(PlantMode)
            SetPlantColors();

        for(int i = 0; i<mapSize; i++)
        {
            SDL_RenderCopy(renderer, tiles[i].img.texture, NULL, &tiles[i].img.destination);
        }


        for(int i = 0; i<buttonCount; i++)
            SDL_RenderCopy(renderer, buttons[i].img.texture, NULL, &buttons[i].img.destination);

        SDL_RenderCopy(renderer, moneyBox.texture, NULL, &moneyBox.destination);
        SDL_RenderCopy(renderer, moneyText.texture, NULL, &moneyText.destination);
        SDL_RenderCopy(renderer, levelText.texture, NULL, &levelText.destination);

        SDL_RenderCopy(renderer, timeBox.texture, NULL, &timeBox.destination);
        SDL_RenderCopy(renderer, timeText.texture, NULL, &timeText.destination);

        SDL_RenderCopy(renderer, buildButton.texture, NULL, &buildButton.destination);
        SDL_RenderCopy(renderer, plantButton.texture, NULL, &plantButton.destination);

        //Az építkezési árak ellenőrzése és átszínezése
        CheckShopItems(&buildPanel, Buildings, BuildChildCount,money,level);
        CheckShopItems(&plantPanel, Plants, PlantChildCount,money,level);

        //Az építő-és ültető menü animált megjelenítése
        ShowAnimatedGUI(renderer, &buildPanel, PanelSpeed, windowSizeY);
        ShowAnimatedGUI(renderer, &plantPanel, PanelSpeed, windowSizeY);

        //"Barkács" menük megjelenítése
        for(int i = 0; i<3; i++)
        {
            if(crafters[i].opened)
                RenderParent(renderer, crafters[i].menu);
        }

        SDL_RenderPresent(renderer);
    }


    SaveStats(level, money, ((int)timer/1000));
    printf("Saving...");

    TTF_CloseFont(font);
    SDL_Quit();

    return 0;
}


void OnLeftMouseDown(SDL_Event e)
{
    LeftMouseDown = true;
    lastMousePos = (Vector2){e.motion.x,e.motion.y};

    for(int i = 0; i<mapSize;i++)
        tiles[i].positionToMouse = (Vector2){e.motion.x-tiles[i].img.destination.x,e.motion.y-tiles[i].img.destination.y};

    SDL_SetTextureColorMod(LastTile -> img.texture,200,200,200);

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
        if(LastTile -> id != NULL)
        {
            SDL_SetTextureColorMod(LastTile -> img.texture,220,220,220);
            printf("[%d](%d:%d) - %s\n",LastTile -> id,LastTile -> coordinates.x, LastTile -> coordinates.y, LastTile -> name);
        }

        level+=1;
        bool clicked = false;


        //Építkezés menü elemeinek kiválasztása
        for(int i =2; i<buildPanel.childCount && !clicked; i+= BuildChildCount)
        {
            if(OverUI((Vector2){e.motion.x, e.motion.y}, buildPanel.children[i].destination) && buildPanel.visible)
            {
                if(Buildings[(i-2)/BuildChildCount].price <= money && Buildings[(i-2)/BuildChildCount].level <= level)
                {
                    BuildMode = !BuildMode;

                    if(BuildMode)
                    {
                        printf("Building selected: %s\n",Buildings[actualBuildItem]);
                        actualBuildItem = (i-2)/5;
                        SetBuildColors();
                    }
                    else
                    {
                        ClearColors();
                        printf("Building cancelled\n");
                    }

                    clicked = true;
                }
                else
                {
                    BuildMode = false;
                }
            }
        }

        //Építkezés menü elemeinek kiválasztása
        for(int i =2; i<plantPanel.childCount && !clicked; i+= PlantChildCount)
        {
            if(OverUI((Vector2){e.motion.x, e.motion.y}, plantPanel.children[i].destination) && plantPanel.visible)
            {
                if(Plants[(i-2)/PlantChildCount].price <= money && Plants[(i-2)/PlantChildCount].level <= level)
                {
                    PlantMode = !PlantMode;

                    if(PlantMode)
                    {
                        printf("Plant selected: %s\n",Plants[actualPlantItem]);
                        actualPlantItem = (i-2)/5;
                        SetPlantColors();
                    }
                    else
                    {
                        ClearColors();
                        printf("Planting cancelled\n");
                    }

                    clicked = true;
                }
                else
                {
                    BuildMode = false;
                }
            }
        }

        if(clicked)
            return;

        //"Barkács" menük megnyitása/bezárása
        for(int i = 0; i<3; i++)
        {
            if(OverUI((Vector2){e.motion.x,e.motion.y}, crafters[i].menu.children[0].destination) && crafters[i].opened)
            {
                crafters[i].opened = false;
            }
            else if(strcmp(LastTile -> name, crafters[i].name) == 0 && !crafters[i].opened && !BuildMode)
            {
                crafters[i].opened = true;
                ClearColors();
                LastTile = &tiles[0];

                CloseBuildPanel();
                ClosePlantPanel();
            }
        }

        if(OverUI((Vector2){e.motion.x,e.motion.y},  buildPanel.children[0].destination) && buildPanel.visible)
        {
            CloseBuildPanel();
        }
        if(OverUI((Vector2){e.motion.x,e.motion.y},  plantPanel.children[0].destination) && plantPanel.visible)
        {
            ClosePlantPanel();
        }

        if(OverUI((Vector2){e.motion.x,e.motion.y},  buildButton.destination) && !buildPanel.visible)
        {
            buildPanel.visible = true;
        }
        if(OverUI((Vector2){e.motion.x,e.motion.y},  plantButton.destination) && !plantPanel.visible)
        {
            plantPanel.visible = true;
        }

        if(BuildMode && strcmp(LastTile -> name, "Ground") == 0)
        {
            char path[30];
            sprintf(&path, "Images/%s.png", Buildings[actualBuildItem].name);

            money -= Buildings[actualBuildItem].price;
            CreateImage(renderer,"Images/ConstructionSite.png",(Vector2){LastTile -> img.destination.x,LastTile -> img.destination.y},(Vector2){baseSize,baseSize}, &tiles[LastTile -> id].img);
            strcpy(tiles[LastTile -> id].name,"ConstructionSite");
            BuildMode = false;
            AddProcess(LastTile -> id,1,Buildings[actualBuildItem].BuildTime, path, Buildings[actualBuildItem].BuildingID);
            ClearColors();
            printf("Build started: %s (%ds)\n",Buildings[actualBuildItem].name, Buildings[actualBuildItem].BuildTime);

        }
        else if(BuildMode && strcmp(Buildings[actualBuildItem].name, "Ground") == 0)
        {
            money -= Buildings[actualBuildItem].price;
            CreateImage(renderer,"Images/ConstructionSite.png",(Vector2){LastTile -> img.destination.x,LastTile -> img.destination.y},(Vector2){baseSize,baseSize}, &tiles[LastTile -> id].img);
            strcpy(tiles[LastTile -> id].name,Buildings[actualBuildItem].name);
            BuildMode = false;
            AddProcess(LastTile -> id,1,Buildings[actualBuildItem].BuildTime, "Images/Ground.png", 10);
            ClearColors();
            printf("Destruction started (%ds)\n",Buildings[actualBuildItem].BuildTime);

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

    for(int i = 0; i<3 ; i++)
        if(crafters[i].opened)
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
            if(tiles[index].id != LastTile -> id)
            {
                SDL_SetTextureColorMod(tiles[index].img.texture,220,220,220);
                SDL_SetTextureColorMod(LastTile -> img.texture,255,255,255);
                LastTile = &tiles[index];
            }

            finded = true;
        }

        index++;
    }

}

void ClosePlantPanel()
{
    plantPanel.visible = false;
}

void CloseBuildPanel()
{
    buildPanel.visible = false;
    BuildMode = false;
    ClearColors();
}

void AddProcess(int id, unsigned char type, int t, char path[], int m)
{
    int i = 0;

    while(BuildProcesses[i].tileID != -1)
    {
        i++;
    }

    BuildProcesses[i].tileID = id;
    BuildProcesses[i].time = t*1000;
    BuildProcesses[i].misc = m;

    strcpy(&BuildProcesses[i].path, path);

    unsigned long int timeToDone =((timer/1000)+t);

    unsigned long long int timeMisc = 0;
    timeMisc += timeToDone;
    timeMisc <<= 8;
    timeMisc += m;

    SetSave(id,type,timeMisc);

    printf("Process added to index: %d Misc: %d Time when done: %lu sec\n",i,BuildProcesses[i].misc,timeToDone);
}

void SetBuildColors()
{
    for(int i = 0; i<mapSize; i++)
    {
        if(BuildMode && strcmp(Buildings[actualBuildItem].name, "Ground") != 0)
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

            while(strcmp(Undestroyables[index],"") != 0 && destroyable)
            {
                destroyable = strcmp(Undestroyables[index],tiles[i].name) != 0;
                index++;
            }

            if(!destroyable)
                SDL_SetTextureColorMod(tiles[i].img.texture,255,150,150);
        }
    }
}
void SetPlantColors()
{
    for(int i = 0; i<mapSize; i++)
    {
        if(PlantMode && strcmp(tiles[i].name,"Field") != 0)
        {
            SDL_SetTextureColorMod(tiles[i].img.texture,255,150,150);
        }
    }
}


void ClearColors()
{
    for(int i = 0; i<mapSize; i++)
    {
        SDL_SetTextureColorMod(tiles[i].img.texture,255,255,255);
    }
}

