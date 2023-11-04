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
#include "ProcessHandler.h"
#include "GameElements.h"

#define d_windowSizeX 1100
#define d_windowSizeY 750
#define d_baseSize 200


typedef struct Crafter
{
    char name[100];
    Tile *base;
    bool opened;
    GUI_Panel menu;
    Process ActiveProcess[3];
} Crafter;

typedef struct Item
{
    char Name[100];
    unsigned int Price;
    unsigned int Amount;
} Item;


//Irányítás
void OnLeftMouseDown(SDL_Event e);
void OnLeftMouseUp(SDL_Event e,Canvas *canvas, Item *AllItem, int ItemCount);
void OnMouseMove(SDL_Event e);

//Színek beállítása
void ClearColors();
void SetBuildColors();
void SetPlantColors();

//Panelek bezárása
void CloseBuildPanel();
void ClosePlantPanel();

ShopItem* GetShopItemById(int id, int n, ShopItem *itemList);
int GetPlantImageId(ShopItem *plant, unsigned long long int TimeWhenDone);
Item* GetItemByName(char *name, int n, Item *itemList);

//Ablak és a renderer
SDL_Window *window;
SDL_Renderer *renderer;

//Fontos adatok
int money = 10000;
int level = 1;
unsigned timer;

int mapSize = 2500;
Tile tiles[2500];

Crafter crafters[3];

bool BuildMode = false;
int actualBuildItem = 0;
ShopItem Buildings[5];
unsigned short int BuildChildCount = 6;
unsigned short int BuildItemCount = 3;
Image BuildLockImages[3];

char *Undestroyables[20] = {"Windmill","Bakery","Brewery","Shop","Warehouse","Ground",""};

bool PlantMode = false;
int actualPlantItem = 0;
ShopItem Plants[4];
unsigned short int PlantChildCount = 6;
unsigned short int PlantItemCount = 4;
Image PlantLockImages[3];

Tile *LastTile;
Vector2 lastMousePos;

ProcessList plist = {NULL,0};

//TileMatrix matrix;

Vector2 origin = {-300,1800};
Vector2 moneyPos;

TTF_Font *font;


    GUI_Panel buildPanel,plantPanel;
    Image buildButton,plantButton,infoBox,moneyText,levelText;

void Init_Map(Vector2 origin)
{
    FILE *file;

    file = fopen("save.bin", "r");
    int rowSize = sqrt(mapSize);

    //Épületek
    Vector2 millPosition ={21,19};
    Vector2 bakeryPosition ={18,21};
    Vector2 breweryPosition ={18,19};
    Vector2 warehousePosition ={23,22};

    if(file != NULL)
    {
        fclose(file);
        unsigned long long int saves[2500];

        GetSave("save.bin",saves);
        level = (saves[0] >> 48);
        timer = ((saves[0] << 32 >> 32)*1000);
        money = (saves[0] << 16 >> 48);

        printf("Save found\n");
        printf("\to Last time: %u sec\n",timer/1000);
        printf("\to Last money: %d\n",money);

        printf("\n\to Saved processes: \n");

        for(int x = 0; x<rowSize;x++)
        {
            for(int y = 0; y<rowSize;y++)
            {

                Vector2 delta = (Vector2){(y*d_baseSize/2)-x*d_baseSize/2,(y*d_baseSize/4)+x*d_baseSize/4};
                delta = (Vector2){delta.x-origin.x, delta.y-origin.y};

                int index = y+x*rowSize;
                int type = saves[index+1] << 12 >> 56;

                if(x==millPosition.x && y==millPosition.y)
                {
                    CreateImage(renderer,"Images/Windmill.png",delta,(Vector2){d_baseSize,d_baseSize}, &tiles[index].img);
                    strcpy(tiles[index].name,"Windmill");
                    crafters[0].base = &tiles[index];
                }
                else if(x==bakeryPosition.x && y==bakeryPosition.y)
                {
                    CreateImage(renderer,"Images/Bakery.png",delta,(Vector2){d_baseSize,d_baseSize}, &tiles[index].img);
                    strcpy(tiles[index].name,"Bakery");
                    crafters[2].base = &tiles[index];
                }
                else if(x==breweryPosition.x && y==breweryPosition.y)
                {
                    CreateImage(renderer,"Images/Brewery.png",delta,(Vector2){d_baseSize,d_baseSize}, &tiles[index].img);
                    strcpy(tiles[index].name,"Brewery");
                    crafters[1].base = &tiles[index];
                }
                else if(x==warehousePosition.x && y==warehousePosition.y)
                {
                    CreateImage(renderer,"Images/Warehouse.png",delta,(Vector2){d_baseSize,d_baseSize}, &tiles[index].img);
                    strcpy(tiles[index].name,"Warehouse");
                }
                else
                {
                    if(type == 11)
                    {
                        CreateImage(renderer,"Images/Tree.png",delta,(Vector2){d_baseSize,d_baseSize}, &tiles[index].img);
                        strcpy(tiles[index].name,"Tree");
                    }
                    else if(type == 10)
                    {
                        CreateImage(renderer,"Images/Ground.png",delta,(Vector2){d_baseSize,d_baseSize}, &tiles[index].img);
                        strcpy(tiles[index].name,"Ground");
                    }
                    else if(type == 1)
                    {
                        CreateImage(renderer,"Images/ConstructionSite.png",delta,(Vector2){d_baseSize,d_baseSize}, &tiles[index].img);
                        strcpy(tiles[index].name,"ConstructionSite");
                        unsigned long long int timeToDone = (saves[index+1] << 20 >> 28);
                        unsigned char buildingType = saves[index+1] << 56 >> 56;

                        printf("\t\t+ %I64u sec is when bulding with type %d is done\n", timeToDone, buildingType);
                        Process p = {index,Building,index,timeToDone,buildingType};
                        AddProcess(&plist, p);
                    }
                    else if(type == 21)
                    {
                        CreateImage(renderer,"Images/Field.png",delta,(Vector2){d_baseSize,d_baseSize}, &tiles[index].img);
                        strcpy(tiles[index].name,"Field");
                        unsigned long long int timeToDone = (saves[index+1] << 20 >> 28);
                        unsigned char plantType = saves[index+1] << 56 >> 56;

                        if(plantType != 0 && timeToDone != 0)
                        {
                            printf("\t\t+ %I64u sec is when plant with type %d is done\n", timeToDone, plantType);
                            Process p = {index,Plant,index,timeToDone,plantType};
                            AddProcess(&plist, p);

                            ShopItem *plant = GetShopItemById(plantType, PlantItemCount, Plants);
                            char pathToPlant[200];
                            sprintf(pathToPlant, "Images/Plants/%s/%s4.png",plant->name,plant->name);
                            CreateImage(renderer, pathToPlant,delta,(Vector2){d_baseSize,d_baseSize}, &tiles[index].additionalImage);
                        }
                    }
                    else
                    {   bool placed = false;

                        for(int i = 0; i<BuildItemCount && !placed;i++)
                        {
                            if(Buildings[i].ID == type)
                            {
                                char path[100];
                                sprintf(path,"Images/%s.png",Buildings[i].name);
                                CreateImage(renderer,path,delta,(Vector2){d_baseSize,d_baseSize}, &tiles[index].img);
                                strcpy(tiles[index].name,Buildings[i].name);
                                placed = true;
                            }
                        }

                        if(!placed)
                        {
                            CreateImage(renderer,"Images/Error.png",delta,(Vector2){d_baseSize,d_baseSize}, &tiles[index].img);
                            strcpy(tiles[index].name,"Error");
                            placed = true;
                        }
                    }


                }


                tiles[index].id = index;
                tiles[index].coordinates = (Vector2){x,y};
                //AddElementToMatrix(&matrix,tiles[index],x,y);
            }
        }
        //printf("Matrix created with %dx%d size\n",matrix.column_size,matrix.row_size);
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

            Vector2 delta = (Vector2){(y*d_baseSize/2)-x*d_baseSize/2,(y*d_baseSize/4)+x*d_baseSize/4};
            delta = (Vector2){delta.x-origin.x, delta.y-origin.y};

            unsigned char type = 0;

            if(x==millPosition.x && y==millPosition.y)
            {
                CreateImage(renderer,"Images/Windmill.png",delta,(Vector2){d_baseSize,d_baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Windmill");
                crafters[0].base = &tiles[y+x*rowSize];

                type = 0;
            }
            else if(x==bakeryPosition.x && y==bakeryPosition.y)
            {
                CreateImage(renderer,"Images/Bakery.png",delta,(Vector2){d_baseSize,d_baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Bakery");
                crafters[2].base = &tiles[y+x*rowSize];

                type = 0;
            }
            else if(x==breweryPosition.x && y==breweryPosition.y)
            {
                CreateImage(renderer,"Images/Brewery.png",delta,(Vector2){d_baseSize,d_baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Brewery");
                crafters[1].base = &tiles[y+x*rowSize];

                type = 0;
            }
            else if(x==warehousePosition.x && y==warehousePosition.y)
            {
                CreateImage(renderer,"Images/Warehouse.png",delta,(Vector2){d_baseSize,d_baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Warehouse");

                type = 0;
            }
            else if(rand()%2==0)
            {
                CreateImage(renderer,"Images/Ground.png",delta,(Vector2){d_baseSize,d_baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Ground");

                type = 10;
            }
            else
            {
                CreateImage(renderer,"Images/Tree.png",delta,(Vector2){d_baseSize,d_baseSize}, &tiles[y+x*rowSize].img);
                strcpy(tiles[y+x*rowSize].name,"Tree");

                type = 11;
            }


            tiles[y+x*rowSize].id = y+x*rowSize;
            tiles[y+x*rowSize].coordinates = (Vector2){x,y};

            unsigned long long int log = GetBinary(y+x*rowSize, type, 0);

            fwrite(&log, sizeof(log), 1, file);
        }

    }

    fclose(file);
}

Canvas Init_GUI()
{

    CreateImage(renderer,"Images/GUI/MoneyBox.png",(Vector2){800,0},(Vector2){300,126}, &infoBox);

    CreateImage(renderer,"Images/GUI/BuildButton.png",(Vector2){d_windowSizeX-100, d_windowSizeY-100},(Vector2){100,100}, &buildButton);
    CreateImage(renderer,"Images/GUI/PlantButton.png",(Vector2){d_windowSizeX-210, d_windowSizeY-100},(Vector2){100,100}, &plantButton);

    //Építő menü létrehozása
    CreateImage(renderer,"Images/GUI/WidePanel.png",(Vector2){0, d_windowSizeY-220},(Vector2){d_windowSizeX,220}, &buildPanel.panelImage);
    CreateImage(renderer,"Images/GUI/Exit.png",(Vector2){d_windowSizeX-60, d_windowSizeY-220},(Vector2){50,50}, &buildPanel.children[buildPanel.childCount]);
    buildPanel.childCount++;
    CreateText("Build",(Color){80,52,30},50,(Vector2){20,d_windowSizeY-270},renderer,font, &buildPanel.children[buildPanel.childCount]);
    buildPanel.childCount++;
    for(int i = 0; i<BuildItemCount; i++)
    {
        CreateImage(renderer,"Images/GUI/Item.png",(Vector2){20+i*160, d_windowSizeY-200},(Vector2){150,180}, &buildPanel.children[buildPanel.childCount]);
        buildPanel.childCount++;
        char path[20];
        sprintf(&path,"Images/%s.png",Buildings[i].name);

        CreateImage(renderer,path ,(Vector2){35+i*160, d_windowSizeY-220},(Vector2){120,120},&buildPanel.children[buildPanel.childCount]);
        buildPanel.childCount++;
        CreateImage(renderer,"Images/GUI/Lock.png" ,(Vector2){40+i*160, d_windowSizeY-200},(Vector2){110,110},&buildPanel.children[buildPanel.childCount]);
        buildPanel.childCount++;

        CreateText(Buildings[i].name,(Color){80,52,30},20,(Vector2){50+i*160,d_windowSizeY-100},renderer,font, &buildPanel.children[buildPanel.childCount]);
        buildPanel.childCount++;
        char additionalText[10];
        sprintf(&additionalText,"$%d",Buildings[i].price);
        CreateText(additionalText,(Color){80,52,30},20,(Vector2){50+i*160,d_windowSizeY-60},renderer,font, &buildPanel.children[buildPanel.childCount]);
        buildPanel.childCount++;
        sprintf(&additionalText,"Level %d",Buildings[i].level);
        CreateText(additionalText,(Color){80,52,30},20,(Vector2){65+i*160,d_windowSizeY-80},renderer,font, &buildPanel.children[buildPanel.childCount]);
        buildPanel.childCount++;

    }

    CreateImage(renderer,"Images/GUI/Bulldozer.png",(Vector2){d_windowSizeX-230, d_windowSizeY-200},(Vector2){150,180}, &buildPanel.children[buildPanel.childCount]);
    buildPanel.childCount++;


    //Ültető menü létrehozása
    CreateImage(renderer,"Images/GUI/WidePanel.png",(Vector2){0, d_windowSizeY-220},(Vector2){d_windowSizeX,220}, &plantPanel.panelImage);
    CreateImage(renderer,"Images/GUI/Exit.png",(Vector2){d_windowSizeX-60, d_windowSizeY-220},(Vector2){50,50}, &plantPanel.children[plantPanel.childCount]);
    plantPanel.childCount++;
    CreateText("Plant",(Color){80,52,30},50,(Vector2){20,d_windowSizeY-270},renderer,font, &plantPanel.children[plantPanel.childCount]);
    plantPanel.childCount++;

    for(int i = 0; i<PlantItemCount; i++)
    {
        CreateImage(renderer,"Images/GUI/Item.png",(Vector2){20+i*160, d_windowSizeY-200},(Vector2){150,180}, &plantPanel.children[plantPanel.childCount]);
        plantPanel.childCount++;
        char path[20];
        sprintf(&path,"Images/Icons/%sseed.png",Plants[i].name,"seed");

        CreateImage(renderer,path ,(Vector2){45+i*160, d_windowSizeY-190},(Vector2){100,100},&plantPanel.children[plantPanel.childCount]);
        plantPanel.childCount++;
        CreateImage(renderer,"Images/GUI/Lock.png" ,(Vector2){40+i*160, d_windowSizeY-200},(Vector2){110,110},&plantPanel.children[plantPanel.childCount]);
        plantPanel.childCount++;
        CreateText(Plants[i].name,(Color){80,52,30},20,(Vector2){50+i*160,d_windowSizeY-100},renderer,font, &plantPanel.children[plantPanel.childCount]);
        plantPanel.childCount++;
        char additionalText[10];
        sprintf(&additionalText,"$%d",Plants[i].price);
        CreateText(additionalText,(Color){80,52,30},20,(Vector2){70+i*160,d_windowSizeY-60},renderer,font, &plantPanel.children[plantPanel.childCount]);
        plantPanel.childCount++;
        sprintf(&additionalText,"Level %d",Plants[i].level);
        CreateText(additionalText,(Color){80,52,30},20,(Vector2){65+i*160,d_windowSizeY-80},renderer,font, &plantPanel.children[plantPanel.childCount]);
        plantPanel.childCount++;

    }


    //A "barkács" épületekhez tartozó menü létrehozása
    CreateCraftPanel(renderer,"Windmill", (Vector2){d_windowSizeX,d_windowSizeY}, &crafters[0].menu);
    CreateCraftPanel(renderer,"Brewery", (Vector2){d_windowSizeX,d_windowSizeY}, &crafters[1].menu);
    CreateCraftPanel(renderer,"Bakery", (Vector2){d_windowSizeX,d_windowSizeY}, &crafters[2].menu);

    return (Canvas){buildPanel, plantPanel, buildButton, plantButton, infoBox, moneyText, levelText};
}

int SelectedTileId;
int SelectInfoAlpha;

int main(int argc, char *argv[])
{
    //Font készlet betöltése
    TTF_Init();
    font = TTF_OpenFont("system.fon", 20);

    Item AllItem[4];
    int itemCount = 4;

    //Inventory
    AllItem[0] = (Item){.Name = "Wheat", .Price = 200, .Amount = 0};
    AllItem[1] = (Item){.Name = "Potato", .Price = 200, .Amount = 0};
    AllItem[2] = (Item){.Name = "Corn", .Price = 200, .Amount = 0};
    AllItem[3] = (Item){.Name = "Tomato", .Price = 200, .Amount = 0};

    //Az építhető épületek deklarálása (név, ár, szint, idő, azonosító)
    Buildings[0] = (ShopItem){"Field",100,1,10,21};
    Buildings[1] = (ShopItem){"Beehive",1000,5,60,22};
    Buildings[2] = (ShopItem){"Greenhouse",5000,10,300,23};
    Buildings[3] = (ShopItem){"Ground",100,1,60,10}; //Megsemmisítés gombhoz
    Buildings[4] = (ShopItem){"ConstructionSite",0,1,60,1}; //Építés alatt álló épületek

    //Ültethető növények deklarálása (név, ár, szint, idő, azonosító)
    Plants[0] = (ShopItem){"Wheat",1,1,30,101};
    Plants[1] = (ShopItem){"Potato",2,2,60,102};
    Plants[2] = (ShopItem){"Corn",5,3,120,103};
    Plants[3] = (ShopItem){"Tomato",10,5,240,104};

    //A "barkács" épületek deklarálása
    crafters[0] = (Crafter){"Windmill"};
    crafters[1] = (Crafter){"Brewery"};
    crafters[2] = (Crafter){"Bakery"};

    //Ablak léterhozása
    CreateWindow("FarmerRealm", d_windowSizeX, d_windowSizeY, &window, &renderer);

    LastTile = &tiles[0];

    Init_Map(origin);
    printf("Map loaded...\n");

    Canvas canvas = Init_GUI();
    printf("GUI loaded...\n");
    printf("Canvas exist %d\n",canvas.plantPanel.childCount);

    //Ha már van elmentett idő akkor ez hozzáadja a mostanihoz
    unsigned plussTime = timer;

    //Háttérszín beállítása
    SDL_SetRenderDrawColor(renderer, 3, 190, 252, 255);

    SDL_Event event;

    //Fő ciklus
    while (event.type != SDL_QUIT)
    {
        //Inputok kezelése
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
                {
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == 1)
                        OnLeftMouseDown(event);
                    break;
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == 1)
                        OnLeftMouseUp(event, &canvas, AllItem, itemCount);
                    break;
                case SDL_MOUSEMOTION:
                    OnMouseMove(event);
                    break;

            }
        }

        //Idő kezelése
        SDL_Delay(10);
        unsigned now;
        unsigned delta_time;

        now = SDL_GetTicks()+plussTime;
        delta_time = now - timer;
        timer += delta_time;

        //Futó folyamatok ellenőrzése

        for(int i = 0; i<plist.n;i++)
        {
            if(plist.l[i].type == Plant && plist.l[i].t >= timer/1000)
            {
                Process *p = &plist.l[i];
                char pathToPlant[200];
                ShopItem *plant = GetShopItemById(p->misc,PlantItemCount,Plants);
                int ImageIndex = GetPlantImageId(plant,p->t);
                sprintf(pathToPlant,"Images/Plants/%s/%s%d.png",plant->name,plant->name,ImageIndex+1);

                Vector2 pos = {tiles[p->TileID].img.destination.x,tiles[p->TileID].img.destination.y};
                SDL_DestroyTexture(&tiles[p->TileID].additionalImage);
                CreateImage(renderer, pathToPlant,pos,(Vector2){d_baseSize,d_baseSize}, &tiles[p->TileID].additionalImage);

                tiles[p->TileID].additional = true;
            }

            if(plist.l[i].t <= now/1000)
            {
                if(plist.l[i].type == Building)
                {
                    int id = 0;

                    for(int k = 0; k<BuildItemCount+1; k++)
                    {
                        if(Buildings[k].ID == plist.l[i].misc)
                        {
                            id = k;
                        }
                    }

                    char path[100];
                    sprintf(path, "Images/%s.png",Buildings[id].name);
                    Vector2 position = {tiles[plist.l[i].TileID].img.destination.x,tiles[plist.l[i].TileID].img.destination.y};
                    strcpy(tiles[plist.l[i].TileID].name,Buildings[id].name);
                    CreateImage(renderer,path, position,(Vector2){d_baseSize,d_baseSize}, &tiles[plist.l[i].TileID].img);
                    SetSave(plist.l[i].TileID,plist.l[i].misc,0);

                    printf("Building of %s (%d) is done\n",Buildings[id].name,Buildings[id].ID);


                    RemoveProcess(&plist, plist.l[i].ProcessID);
                }
            }
        }


        Image selectImage;
        Tile select = tiles[SelectedTileId];
        Process *selectedProcess = GetProcess(&plist,SelectedTileId);
        char infos[100];

        if(selectedProcess != NULL)
        {
            int timeLeft = (selectedProcess->t-(timer/1000));
            char formattedTime[30];
            FormatTime(timeLeft*1000, formattedTime);
            sprintf(infos,"%s (%s)",select.name,formattedTime);
        }
        else
        {
            sprintf(infos,"%s",select.name);
        }

        CreateText(infos, (Color){80,52,30}, 30, (Vector2){d_windowSizeX/2-200,50}, renderer, font, &selectImage);
        SDL_SetTextureAlphaMod(selectImage.texture, SelectInfoAlpha);

        if(SelectInfoAlpha > 1)
            SelectInfoAlpha-=delta_time/10;
        else
            SelectInfoAlpha = 0;


        //Pénz és szint megjelenítése
        char intToString[20];

        sprintf(intToString, "$%d", money);
        SDL_DestroyTexture(&canvas.moneyText);
        CreateText(intToString, (Color){80,52,30}, 50, (Vector2){880,23}, renderer, font, &canvas.moneyText);

        sprintf(intToString, "Level %d", level);
        SDL_DestroyTexture(&canvas.moneyText);
        CreateText(intToString, (Color){80,52,30}, 30, (Vector2){880,85}, renderer, font, &canvas.levelText);


        SDL_RenderClear(renderer);

        //Ellenőrzi hogy éppen milyen módban van a játék
        if(BuildMode)
            SetBuildColors();

        if(PlantMode)
            SetPlantColors();

        //Térkép elemek megjelenítése
        for(int i = 0; i<mapSize; i++)
        {
            SDL_RenderCopy(renderer, tiles[i].img.texture, NULL, &tiles[i].img.destination);

            if(tiles[i].additional)
            {
                SDL_RenderCopy(renderer, tiles[i].additionalImage.texture, NULL, &tiles[i].additionalImage.destination);
            }
        }

        SDL_RenderCopy(renderer, selectImage.texture, NULL, &selectImage.destination);

        RenderCanvas(&canvas, renderer);

        //Az építkezési árak ellenőrzése és átszínezése
        CheckShopItems(&canvas.buildPanel, Buildings, BuildChildCount,money,level);
        CheckShopItems(&canvas.plantPanel, Plants, PlantChildCount,money,level);

        //Az építő-és ültető menü animált megjelenítése
        ShowAnimatedGUI(renderer, &canvas.buildPanel, d_windowSizeY);
        ShowAnimatedGUI(renderer, &canvas.plantPanel, d_windowSizeY);

        //"Barkács" menük megjelenítése
        for(int i = 0; i<3; i++)
        {
            if(crafters[i].opened)
                RenderParent(renderer, crafters[i].menu);
        }

        SDL_RenderPresent(renderer);
    }

    free(plist.l);

    //Adatok mentése
    SaveStats(level, money, ((int)timer/1000));
    printf("Saving...\n");

    //Fennmaradó dolgok bezárása
    TTF_CloseFont(font);
    SDL_Quit();

    return 0;
}


bool LeftMouseDown;

void OnLeftMouseDown(SDL_Event e)
{
    LeftMouseDown = true;
    lastMousePos = (Vector2){e.motion.x,e.motion.y};

    for(int i = 0; i<mapSize;i++)
        tiles[i].positionToMouse = (Vector2){e.motion.x-tiles[i].img.destination.x,e.motion.y-tiles[i].img.destination.y};

    SDL_SetTextureColorMod(LastTile -> img.texture,200,200,200);
}

void OnLeftMouseUp(SDL_Event e,Canvas *canvas, Item *AllItem, int ItemCount)
{
    LeftMouseDown = false;

    //Kiszámítjuk az egérrel megtett út hosszát, hogy megállapítsuk, hogy click volt-e vagy vonszolás
    double moved = sqrt(pow(lastMousePos.x-e.motion.x,2)+pow(lastMousePos.y-e.motion.y,2));


    if(moved < 10)
    {
        if(LastTile != NULL)
        {
            SDL_SetTextureColorMod(LastTile -> img.texture,220,220,220);
            printf("[%d](%d:%d) - %s\n",LastTile -> id,LastTile -> coordinates.x, LastTile -> coordinates.y, LastTile -> name);
        }

        bool clicked = false;

        SelectedTileId = LastTile ->id;
        SelectInfoAlpha = 255;

        //Építkezés menü elemeinek kiválasztása
        for(int i =2; i<canvas->buildPanel.childCount && !clicked; i+= BuildChildCount)
        {
            if(OverUI((Vector2){e.motion.x, e.motion.y}, canvas->buildPanel.children[i].destination) && canvas->buildPanel.visible)
            {
                if(Buildings[(i-2)/BuildChildCount].price <= money && Buildings[(i-2)/BuildChildCount].level <= level)
                {
                    BuildMode = !BuildMode;

                    if(BuildMode)
                    {
                        actualBuildItem = (i-2)/5;
                        printf("Building selected: %s\n",Buildings[actualBuildItem].name);
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

        //Ültetés menü elemeinek kiválasztása
        for(int i =2; i<canvas->plantPanel.childCount && !clicked; i+= PlantChildCount)
        {
            if(OverUI((Vector2){e.motion.x, e.motion.y}, canvas->plantPanel.children[i].destination) && canvas->plantPanel.visible)
            {
                if(Plants[(i-2)/PlantChildCount].price <= money && Plants[(i-2)/PlantChildCount].level <= level)
                {
                    PlantMode = !PlantMode;

                    if(PlantMode)
                    {
                        actualPlantItem = (i-2)/5;
                        printf("Plant selected: %s\n",Plants[actualPlantItem].name);
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

                canvas->plantPanel.visible = false;
                canvas->buildPanel.visible = false;
                BuildMode = false;
                ClearColors();
            }
        }

        if(OverUI((Vector2){e.motion.x,e.motion.y},  canvas->buildPanel.children[0].destination) && canvas->buildPanel.visible)
        {
            canvas->buildPanel.visible = false;
            BuildMode = false;
            ClearColors();
        }
        if(OverUI((Vector2){e.motion.x,e.motion.y},  canvas->plantPanel.children[0].destination) && canvas->plantPanel.visible)
        {
            canvas->plantPanel.visible = false;
        }

        if(OverUI((Vector2){e.motion.x,e.motion.y},  canvas->buildButton.destination) && !canvas->buildPanel.visible)
        {
            canvas->buildPanel.visible = true;
        }
        if(OverUI((Vector2){e.motion.x,e.motion.y},  canvas->plantButton.destination) && !canvas->plantPanel.visible)
        {
            canvas->plantPanel.visible = true;
        }

        if(BuildMode && (strcmp(LastTile -> name, "Ground") == 0 || strcmp(Buildings[actualBuildItem].name, "Ground") == 0))
        {
            money-= Buildings[actualBuildItem].price;

            unsigned int id = LastTile->id;
            unsigned long long int t = (timer/1000)+Buildings[actualBuildItem].time;

            Process p = {id,Building,id,t,Buildings[actualBuildItem].ID};
            AddProcess(&plist,p);

            Vector2 position = {LastTile->img.destination.x,LastTile->img.destination.y};

            CreateImage(renderer,"Images/ConstructionSite.png", position,(Vector2){d_baseSize,d_baseSize}, &LastTile->img);
            strcpy(LastTile->name,"ConstructionSite");


            BuildMode = false;

            ClearColors();

            unsigned long long int misc = t;
            misc <<= 8;
            misc += Buildings[actualBuildItem].ID;

            SetSave(id,1,misc);

            printf("%s(%d) building started (%d sec)\n",Buildings[actualBuildItem].name,Buildings[actualBuildItem].ID,Buildings[actualBuildItem].time);
        }

        if(PlantMode && strcmp(LastTile -> name, "Field") == 0)
        {
            money -= Plants[actualPlantItem].price;

            unsigned int id = LastTile->id;
            unsigned long long int t = (timer/1000)+Plants[actualPlantItem].time;

            Process p = {id,Plant,id,t,Plants[actualPlantItem].ID};
            AddProcess(&plist,p);

            PlantMode = false;
            ClearColors();

            unsigned long long int misc = t;
            misc <<= 8;
            misc += Plants[actualPlantItem].ID;

            SetSave(id,21,misc);

            printf("%s placed on field (%d:%d)",Plants[actualPlantItem].name, LastTile ->coordinates.x, LastTile ->coordinates.y);
        }

        Process *selectedProcess = GetProcess(&plist, LastTile->id);
        if(selectedProcess != NULL && selectedProcess->t < timer/1000)
        {
            ShopItem *plant = GetShopItemById(selectedProcess->misc,PlantItemCount, Plants);
            printf("Product (%s) is done\n",plant->name);
            Item *item = GetItemByName(plant->name, ItemCount, AllItem);
            item->Amount++;

            printf("Actual inventory:\n");
            for(int i = 0; i<ItemCount;i++)
                printf("\to %s: %d\n",AllItem[i].Name, AllItem[i].Amount);


            SetSave(selectedProcess->TileID, 21,0);
            SDL_DestroyTexture(tiles[selectedProcess->TileID].additionalImage.texture);
            tiles[selectedProcess->TileID].additional = false;
            RemoveProcess(&plist,selectedProcess->ProcessID);
        }
    }
}

void OnMouseMove(SDL_Event e)
{
    //Ha bármilyen panel meg van nyitva akkor kilép

    for(int i = 0; i<3 ; i++)
        if(crafters[i].opened)
            return;


    //Térkép mozgatása
    for(int i = 0; i<mapSize && LeftMouseDown; i++)
    {
        tiles[i].img.destination.x = (e.motion.x-tiles[i].positionToMouse.x);
        tiles[i].img.destination.y = (e.motion.y-tiles[i].positionToMouse.y);
        tiles[i].additionalImage.destination.x = (e.motion.x-tiles[i].positionToMouse.x);
        tiles[i].additionalImage.destination.y = (e.motion.y-tiles[i].positionToMouse.y);

    }


    //Kép színének megváltoztatása ha rajta az egér
    bool finded = false;
    int index = 0;

    while(!finded && index < mapSize)
    {
        if(MouseOverImage(tiles[index].img.destination, (Vector2){e.motion.x,e.motion.y}, d_baseSize))
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

//Az aktuális növény képének kiszámolása a hátralévő időből
int GetPlantImageId(ShopItem *plant, unsigned long long int TimeWhenDone)
{
    unsigned long long TimeLeft = TimeWhenDone-(timer/1000);
    unsigned int AllTime = plant->time;
    unsigned short int PictureTime = AllTime/3;


    return (AllTime-TimeLeft)/PictureTime;
}

//Megkeresi azonosító alapján a tárgyat a tömbben
ShopItem* GetShopItemById(int id, int n, ShopItem *itemList)
{
    for(int i = 0; i<n; i++)
    {
        if(itemList[i].ID == id)
            return &itemList[i];
    }
    return NULL;
}

Item* GetItemByName(char *name, int n, Item *itemList)
{
    for(int i = 0; i<n;i++)
    {
        if(strcmp(name, itemList[i].Name) == 0)
            return &itemList[i];
    }

    return NULL;
}
