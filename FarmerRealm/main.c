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
#include "debugmalloc.h"

#define d_windowSizeX 1100
#define d_windowSizeY 750
#define d_baseSize 200
#define d_mapSize 2500


typedef struct Crafter
{
    char name[100];
    Tile *base;
    bool opened;
    GUI_Panel menu;
    int ProcessIds[3];
    RecipeType type;
} Crafter;

//Ir�ny�t�s
void OnLeftMouseDown(SDL_Event e);
void OnLeftMouseUp(SDL_Event e,Canvas *canvas, Item *AllItem, int ItemCount,int* ids, ShopData* plantData, ShopData* buildData, Crafter *crafters, bool *BuildMode, bool *PlantMode, ArrayData *recipeData, ArrayData *invData);
void OnMouseMove(SDL_Event e,Item *AllItem, int ItemCount, Crafter *crafters, bool LeftMouseDown);

//Inventory kezel� f�ggv�nyek
ShopItem* GetShopItemById(int id, int n, ShopItem *itemList);
int GetPlantImageId(ShopItem *plant, unsigned long long int TimeWhenDone);
Item* GetItemByName(char *name, int n, Item *itemList);

//P�nz �s szint kezel�se
void AddXP(int amount, Image *levelText, Image *levelProg);
void AddMoney(int amount, Image *moneyText);

//Ablak �s a renderer
SDL_Window *window;
SDL_Renderer *renderer;

//Fontos adatok
int money = 200;
int level = 1;
int xp = 0;
unsigned timer;

Tile *LastTile;
ProcessList plist = {NULL,0};

TTF_Font *font;
TileMatrix matrix;

void Init_Map(ShopData *plantData, ShopData *buildData, Crafter *crafters)
{
    Vector2 origin = {-300,1800};
    FILE *file;
    xp = 0;
    file = fopen("save.bin", "r");
    int rowSize = sqrt(d_mapSize);

    matrix.xSize = rowSize;
    matrix.ySize = rowSize;

    matrix.matrix = CreateMatrix(rowSize,rowSize);

    //�p�letek
    Vector2 millPosition ={21,19};
    Vector2 bakeryPosition ={18,21};
    Vector2 breweryPosition ={18,19};
    Vector2 warehousePosition ={23,22};

    if(file != NULL)
    {
        unsigned long long int saves[2509];

        GetSave("save.bin",saves);
        xp = (saves[0] >> 48);
        timer = ((saves[0] << 32 >> 32)*1000);
        money = (saves[0] << 16 >> 48);

        time_t aTime;
        time(&aTime);
        unsigned long long int actualTime = aTime;
        unsigned long long int lastTime = GetLastTime();

        if(actualTime > lastTime && lastTime != 0)
        {
            int since = (actualTime-lastTime);
            timer += since*1000;
            printf("Since last time: %d sec\n",since);
        }

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
                matrix.matrix[x][y].additionalImage.texture = NULL;
                matrix.matrix[x][y].icon.texture = NULL;
                matrix.matrix[x][y].arrow = false;
                matrix.matrix[x][y].process = NULL;

                if(x==millPosition.x && y==millPosition.y)
                {
                    CreateImage(renderer,"Images/Windmill.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                    strcpy(matrix.matrix[x][y].name,"Windmill");
                    crafters[0].base = &matrix.matrix[x][y];
                    printf("Windmill created\n");

                    for(int i = 1; i<=3;i++)
                    {
                        int type = saves[d_mapSize+i] << 12 >> 56;

                        if(type != 0)
                        {
                            unsigned long long int timeToDone = (saves[d_mapSize+i] << 20 >> 28);
                            unsigned char recipe = saves[d_mapSize+i] << 56 >> 56;

                            if(timeToDone != 0 && type != 0)
                            {
                                Process p = {index*i,Craft,index,timeToDone,recipe};
                                crafters[0].ProcessIds[i-1] = AddProcess(&plist, p)->ProcessID;
                            }
                        }
                    }

                }
                else if(x==bakeryPosition.x && y==bakeryPosition.y)
                {
                    CreateImage(renderer,"Images/Bakery.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                    strcpy(matrix.matrix[x][y].name,"Bakery");
                    crafters[2].base = &matrix.matrix[x][y];
                    printf("Bakery created\n");

                    for(int i = 1; i<=3;i++)
                    {
                        int type = saves[d_mapSize+6+i] << 12 >> 56;

                        if(type != 0)
                        {
                            unsigned long long int timeToDone = (saves[d_mapSize+6+i] << 20 >> 28);
                            unsigned char recipe = saves[d_mapSize+6+i] << 56 >> 56;

                            if(timeToDone != 0 && type != 0)
                            {
                                Process p = {index*i,Craft,index,timeToDone,recipe};
                                crafters[2].ProcessIds[i-1] = AddProcess(&plist, p)->ProcessID;
                            }
                        }
                    }
                }
                else if(x==breweryPosition.x && y==breweryPosition.y)
                {
                    CreateImage(renderer,"Images/Brewery.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                    strcpy(matrix.matrix[x][y].name,"Brewery");
                    crafters[1].base = &matrix.matrix[x][y];
                    printf("Brewery created\n");

                    for(int i = 1; i<=3;i++)
                    {
                        int type = saves[d_mapSize+3+i] << 12 >> 56;

                        if(type != 0)
                        {
                            unsigned long long int timeToDone = (saves[d_mapSize+3+i] << 20 >> 28);
                            unsigned char recipe = saves[d_mapSize+3+i] << 56 >> 56;

                            if(timeToDone != 0 && type != 0)
                            {
                                Process p = {index*i,Craft,index,timeToDone,recipe};
                                crafters[1].ProcessIds[i-1] = AddProcess(&plist, p)->ProcessID;
                            }
                        }
                    }
                }
                else if(x==warehousePosition.x && y==warehousePosition.y)
                {
                    CreateImage(renderer,"Images/Warehouse.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                    strcpy(matrix.matrix[x][y].name,"Warehouse");
                    printf("Warehouse created\n");
                }
                else
                {
                    if(type == 11)
                    {
                        CreateImage(renderer,"Images/Tree.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                        strcpy(matrix.matrix[x][y].name,"Tree");
                    }
                    else if(type == 10)
                    {
                        CreateImage(renderer,"Images/Ground.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                        strcpy(matrix.matrix[x][y].name,"Ground");
                    }
                    else if(type == 1)
                    {
                        CreateImage(renderer,"Images/ConstructionSite.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                        strcpy(matrix.matrix[x][y].name,"ConstructionSite");
                        unsigned long long int timeToDone = (saves[index+1] << 20 >> 28);
                        unsigned char buildingType = saves[index+1] << 56 >> 56;

                        printf("\t\t+ %I64u sec is when bulding with type %d is done\n", timeToDone, buildingType);
                        Process p = {index,Building,index,timeToDone,buildingType};
                        matrix.matrix[x][y].process = AddProcess(&plist, p);
                    }
                    else if(type == 21)
                    {
                        CreateImage(renderer,"Images/Field.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                        strcpy(matrix.matrix[x][y].name,"Field");
                        unsigned long long int timeToDone = (saves[index+1] << 20 >> 28);
                        unsigned char plantType = saves[index+1] << 56 >> 56;

                        if(plantType != 0 && timeToDone != 0)
                        {
                            printf("\t\t+ %I64u sec is when plant with type %d is done\n", timeToDone, plantType);
                            Process p = {index,Plant,index,timeToDone,plantType};
                            matrix.matrix[x][y].process = AddProcess(&plist, p);

                            ShopItem *plant = GetShopItemById(plantType, plantData->ItemCount, plantData->Items);
                            char pathToPlant[200];
                            sprintf(pathToPlant, "Images/Plants/%s/%s4.png",plant->name,plant->name);
                            CreateImage(renderer, pathToPlant,delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].additionalImage);
                        }
                    }
                    else if(type == 22)
                    {
                        CreateImage(renderer,"Images/Beehive.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                        strcpy(matrix.matrix[x][y].name,"Beehive");
                        unsigned long long int timeToDone = (saves[index+1] << 20 >> 28);
                        unsigned char productType = saves[index+1] << 56 >> 56;

                        if(timeToDone != 0)
                        {
                            printf("\t\t+ %I64u sec is when looped production with type %d is done\n", timeToDone, productType);
                            Process p = {index,Loop,index,timeToDone,productType};
                            matrix.matrix[x][y].process = AddProcess(&plist, p);
                        }
                        else
                        {
                            timeToDone = timer/1000+600;
                            printf("Beehive looped production started (%d)\n",timeToDone);
                            Process p = {index,Loop,index,timeToDone,6};
                            matrix.matrix[x][y].process = AddProcess(&plist, p);

                            unsigned long long int misc = timeToDone;
                            misc <<= 8;
                            misc += 6;

                            SetSave(index, 22, misc);
                        }
                    }
                    else if(type == 23)
                    {
                        CreateImage(renderer,"Images/Greenhouse.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                        strcpy(matrix.matrix[x][y].name,"Greenhouse");
                        unsigned long long int timeToDone = (saves[index+1] << 20 >> 28);
                        unsigned char plantType = saves[index+1] << 56 >> 56;

                        if(plantType != 0 && timeToDone != 0)
                        {
                            printf("\t\t+ %I64u sec is when plant with type %d is done\n", timeToDone, plantType);
                            Process p = {index,Plant,index,timeToDone,plantType};
                            matrix.matrix[x][y].process = AddProcess(&plist, p);
                        }
                    }
                    else
                    {
                        bool placed = false;

                        for(int i = 0; i<buildData->ItemCount && !placed;i++)
                        {
                            if(buildData->Items[i].ID == type)
                            {
                                char path[100];
                                sprintf(path,"Images/%s.png",buildData->Items[i].name);
                                CreateImage(renderer,path,delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                                strcpy(matrix.matrix[x][y].name,buildData->Items[i].name);
                                placed = true;
                            }
                        }

                        if(!placed)
                        {
                            CreateImage(renderer,"Images/Ground.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                            strcpy(matrix.matrix[x][y].name,"Ground");
                            printf("Error while loading tile(%d:%d) with type: %d\n",x,y,type);
                            placed = true;
                        }
                    }


                }

                matrix.matrix[x][y].id = index;
                matrix.matrix[x][y].coordinates = (Vector2){x,y};
            }
        }

        fclose(file);
        return;
    }

    fclose(file);
    file = fopen("save.bin", "wb");

    unsigned long long int stats = 0;
    stats += 100;
    stats <<= 16;

    stats += money;
    stats <<= 32;

    fwrite(&stats, sizeof(stats), 1, file);

    for(int x = 0; x<rowSize;x++)
    {
        for(int y = 0; y<rowSize;y++)
        {

            Vector2 delta = (Vector2){(y*d_baseSize/2)-x*d_baseSize/2,(y*d_baseSize/4)+x*d_baseSize/4};
            delta = (Vector2){delta.x-origin.x, delta.y-origin.y};


            matrix.matrix[x][y].additionalImage.texture = NULL;
            matrix.matrix[x][y].icon.texture = NULL;
            matrix.matrix[x][y].arrow = false;

            unsigned char type = 0;

            if(x==millPosition.x && y==millPosition.y)
            {
                CreateImage(renderer,"Images/Windmill.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                strcpy(matrix.matrix[x][y].name,"Windmill");
                crafters[0].base = &matrix.matrix[x][y];

                type = 0;
            }
            else if(x==bakeryPosition.x && y==bakeryPosition.y)
            {
                CreateImage(renderer,"Images/Bakery.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                strcpy(matrix.matrix[x][y].name,"Bakery");
                crafters[2].base = &matrix.matrix[x][y];

                type = 0;
            }
            else if(x==breweryPosition.x && y==breweryPosition.y)
            {
                CreateImage(renderer,"Images/Brewery.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                strcpy(matrix.matrix[x][y].name,"Brewery");
                crafters[1].base = &matrix.matrix[x][y];

                type = 0;
            }
            else if(x==warehousePosition.x && y==warehousePosition.y)
            {
                CreateImage(renderer,"Images/Warehouse.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                strcpy(matrix.matrix[x][y].name,"Warehouse");

                type = 0;
            }
            else if(rand()%2==0)
            {
                CreateImage(renderer,"Images/Ground.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                strcpy(matrix.matrix[x][y].name,"Ground");

                type = 10;
            }
            else
            {
                CreateImage(renderer,"Images/Tree.png",delta,(Vector2){d_baseSize,d_baseSize}, &matrix.matrix[x][y].img);
                strcpy(matrix.matrix[x][y].name,"Tree");

                type = 11;
            }


            matrix.matrix[x][y].id = y+x*rowSize;
            matrix.matrix[x][y].coordinates = (Vector2){x,y};

            unsigned long long int log = GetBinary(y+x*rowSize, type, 0);

            fwrite(&log, sizeof(log), 1, file);
        }

    }

    fclose(file);
}

void Init_GUI(Canvas* canvas, ShopData *plantData, ShopData *buildData)
{
    Color textColor = (Color){80,52,30};
    canvas->buildPanel.childCount = 0;
    canvas->plantPanel.childCount = 0;
    canvas->warehousePanel.childCount = 0;

    canvas->warehousePanel.visible = false;
    canvas->plantPanel.visible = false;
    canvas->buildPanel.visible = false;

    CreateImage(renderer,"Images/GUI/MoneyBox.png",(Vector2){800,0},(Vector2){300,126}, &canvas->infoBox);

    CreateImage(renderer,"Images/GUI/BuildButton.png",(Vector2){d_windowSizeX-100, d_windowSizeY-100},(Vector2){100,100}, &canvas->buildButton);
    CreateImage(renderer,"Images/GUI/PlantButton.png",(Vector2){d_windowSizeX-210, d_windowSizeY-100},(Vector2){100,100}, &canvas->plantButton);

    //�p�t� men� l�trehoz�sa
    CreateImage(renderer,"Images/GUI/WidePanel.png",(Vector2){0, d_windowSizeY-220},(Vector2){d_windowSizeX,220}, &canvas->buildPanel.panelImage);
    CreateImage(renderer,"Images/GUI/Exit.png",(Vector2){d_windowSizeX-60, d_windowSizeY-220},(Vector2){50,50}, &canvas->buildPanel.children[canvas->buildPanel.childCount]);

    canvas->buildPanel.childCount++;
    CreateText("Build",textColor,50,(Vector2){20,d_windowSizeY-270},renderer,font, &canvas->buildPanel.children[canvas->buildPanel.childCount]);
    canvas->buildPanel.childCount++;


    for(int i = 0; i<buildData->ItemCount; i++)
    {
        CreateImage(renderer,"Images/GUI/Item.png",(Vector2){20+i*160, d_windowSizeY-200},(Vector2){150,180}, &canvas->buildPanel.children[canvas->buildPanel.childCount]);
        canvas->buildPanel.childCount++;
        char path[20];
        sprintf(&path,"Images/%s.png",buildData->Items[i].name);

        CreateImage(renderer,path ,(Vector2){35+i*160, d_windowSizeY-220},(Vector2){120,120},&canvas->buildPanel.children[canvas->buildPanel.childCount]);
        canvas->buildPanel.childCount++;
        CreateImage(renderer,"Images/GUI/Lock.png" ,(Vector2){40+i*160, d_windowSizeY-200},(Vector2){110,110},&canvas->buildPanel.children[canvas->buildPanel.childCount]);
        canvas->buildPanel.childCount++;

        CreateText(buildData->Items[i].name,textColor,20,(Vector2){50+i*160,d_windowSizeY-100},renderer,font, &canvas->buildPanel.children[canvas->buildPanel.childCount]);
        canvas->buildPanel.childCount++;
        char additionalText[10];
        sprintf(&additionalText,"$%d",buildData->Items[i].price);
        CreateText(additionalText,textColor,20,(Vector2){50+i*160,d_windowSizeY-60},renderer,font, &canvas->buildPanel.children[canvas->buildPanel.childCount]);
        canvas->buildPanel.childCount++;
        sprintf(&additionalText,"Level %d",buildData->Items[i].level);
        CreateText(additionalText,textColor,20,(Vector2){65+i*160,d_windowSizeY-80},renderer,font, &canvas->buildPanel.children[canvas->buildPanel.childCount]);
        canvas->buildPanel.childCount++;

    }

    CreateImage(renderer,"Images/GUI/Bulldozer.png",(Vector2){d_windowSizeX-230, d_windowSizeY-200},(Vector2){150,180}, &canvas->buildPanel.children[canvas->buildPanel.childCount]);
    canvas->buildPanel.childCount++;

    //�ltet� men� l�trehoz�sa
    CreateImage(renderer,"Images/GUI/WidePanel.png",(Vector2){0, d_windowSizeY-220},(Vector2){d_windowSizeX,220}, &canvas->plantPanel.panelImage);
    CreateImage(renderer,"Images/GUI/Exit.png",(Vector2){d_windowSizeX-60, d_windowSizeY-220},(Vector2){50,50}, &canvas->plantPanel.children[canvas->plantPanel.childCount]);
    canvas->plantPanel.childCount++;
    CreateText("Plant",textColor,50,(Vector2){20,d_windowSizeY-270},renderer,font, &canvas->plantPanel.children[canvas->plantPanel.childCount]);
    canvas->plantPanel.childCount++;

    for(int i = 0; i<plantData->ItemCount; i++)
    {
        CreateImage(renderer,"Images/GUI/Item.png",(Vector2){20+i*160, d_windowSizeY-200},(Vector2){150,180}, &canvas->plantPanel.children[canvas->plantPanel.childCount]);
        canvas->plantPanel.childCount++;
        char path[20];
        sprintf(&path,"Images/Icons/%sseed.png",plantData->Items[i].name,"seed");

        CreateImage(renderer,path ,(Vector2){45+i*160, d_windowSizeY-190},(Vector2){100,100},&canvas->plantPanel.children[canvas->plantPanel.childCount]);
        canvas->plantPanel.childCount++;
        CreateImage(renderer,"Images/GUI/Lock.png" ,(Vector2){40+i*160, d_windowSizeY-200},(Vector2){110,110},&canvas->plantPanel.children[canvas->plantPanel.childCount]);
        canvas->plantPanel.childCount++;
        CreateText(plantData->Items[i].name,textColor,20,(Vector2){50+i*160,d_windowSizeY-100},renderer,font, &canvas->plantPanel.children[canvas->plantPanel.childCount]);
        canvas->plantPanel.childCount++;
        char additionalText[60];
        sprintf(&additionalText,"$%d",plantData->Items[i].price);
        CreateText(additionalText,textColor,20,(Vector2){70+i*160,d_windowSizeY-60},renderer,font, &canvas->plantPanel.children[canvas->plantPanel.childCount]);
        canvas->plantPanel.childCount++;
        sprintf(&additionalText,"Level %d",plantData->Items[i].level);
        CreateText(additionalText,textColor,20,(Vector2){65+i*160,d_windowSizeY-80},renderer,font, &canvas->plantPanel.children[canvas->plantPanel.childCount]);
        canvas->plantPanel.childCount++;

    }
    CreateImage(renderer, "Images/GUI/BigPanel.png",(Vector2){100,100},(Vector2){700,600}, &canvas->warehousePanel.panelImage);
    CreateImage(renderer,"Images/GUI/Exit.png",(Vector2){725, 100},(Vector2){75,75}, &canvas->warehousePanel.children[canvas->warehousePanel.childCount]);
    canvas->warehousePanel.childCount++;
    CreateText("Warehouse",textColor,60,(Vector2){310,150},renderer,font,&canvas->warehousePanel.children[canvas->warehousePanel.childCount]);
    canvas->warehousePanel.childCount++;


    CreateImage(renderer, "Images/GUI/Progression.png",(Vector2){844,85},(Vector2){215,29}, &canvas->levelProgression);
    SDL_SetTextureAlphaMod(canvas->levelProgression.texture, 100);

    canvas->moneyText.texture = NULL;
    canvas->levelText.texture = NULL;


    //P�nz �s szint sz�veg friss�t�se
    AddMoney(0, &canvas->moneyText);
    AddXP(0, &canvas->levelText, &canvas->levelProgression);
}

int main(int argc, char *argv[])
{
    //Font k�szlet bet�lt�se
    //A bet�k�szlet Microsoft Corporation �ltal 8514syse.fon Windows 10-ben megtal�lhat� el�re telep�tett
    TTF_Init();
    font = TTF_OpenFont("System.fon", 20);


    ArrayData RecipesData;
    ArrayData InventoryData;

    int ItemCount = 13;
    Item AllItem[ItemCount];

    //Inventory
    AllItem[0] = (Item){.Name = "Wheat", .Price = 3, .Amount = 0, .id = 0};
    AllItem[1] = (Item){.Name = "Potato", .Price = 5, .Amount = 0, .id = 1};
    AllItem[2] = (Item){.Name = "Corn", .Price = 6, .Amount = 0, .id = 2};
    AllItem[3] = (Item){.Name = "Tomato", .Price = 6, .Amount = 0, .id = 3};
    AllItem[4] = (Item){.Name = "Lettuce", .Price = 9, .Amount = 0, .id = 4};
    AllItem[5] = (Item){.Name = "Pumpkin", .Price = 30, .Amount = 0, .id = 5};
    AllItem[6] = (Item){.Name = "Honey", .Price = 15, .Amount = 0, .id = 6};
    AllItem[7] = (Item){.Name = "Flour", .Price = 10, .Amount = 0, .id = 7};
    AllItem[8] = (Item){.Name = "Bread", .Price = 30, .Amount = 0, .id = 8};
    AllItem[9] = (Item){.Name = "Ketchup", .Price = 20, .Amount = 0, .id = 9};
    AllItem[10] = (Item){.Name = "Pumpkinpie", .Price = 30, .Amount = 0, .id = 10};
    AllItem[11] = (Item){.Name = "Vodka", .Price = 40, .Amount = 0, .id = 11};
    AllItem[12] = (Item){.Name = "Bourbon", .Price = 50, .Amount = 0, .id = 12};

    GetInventory(AllItem, ItemCount);

    InventoryData = (ArrayData){AllItem, ItemCount};

    ShopData BuildingData;
    ShopData PlantingData;

    //Az �p�thet� �p�letek deklar�l�sa (n�v, �r, szint, id�, azonos�t�)

    BuildingData.Items[0] = (ShopItem){"Field",30,1,10,21};
    BuildingData.Items[1] = (ShopItem){"Beehive",500,5,60,22};
    BuildingData.Items[2] = (ShopItem){"Greenhouse",2000,10,300,23};
    BuildingData.Items[3] = (ShopItem){"Ground",10,1,60,10}; //Megsemmis�t�s gombhoz
    BuildingData.Items[4] = (ShopItem){"ConstructionSite",0,1,60,1}; //�p�t�s alatt �ll� �p�letek

    BuildingData.ItemCount = 3;
    BuildingData.ChildCount = 6;
    BuildingData.actualItem = 0;

    //�ltethet� n�v�nyek deklar�l�sa (n�v, �r, szint, id�, azonos�t�)
    PlantingData.Items[0] = (ShopItem){"Wheat",1,1,30,101};
    PlantingData.Items[1] = (ShopItem){"Potato",2,2,60,102};
    PlantingData.Items[2] = (ShopItem){"Corn",4,3,120,103};
    PlantingData.Items[3] = (ShopItem){"Tomato",4,5,240,104};
    PlantingData.Items[4] = (ShopItem){"Lettuce",4,6,60,105};
    PlantingData.Items[5] = (ShopItem){"Pumpkin",20,8,300,106};

    PlantingData.ItemCount = 6;
    PlantingData.ChildCount = 6;
    PlantingData.actualItem = 0;

    //A "bark�cs" �p�letek deklar�l�sa
    Crafter crafters[3];

    crafters[0] = (Crafter){"Windmill"};
    crafters[1] = (Crafter){"Brewery"};
    crafters[2] = (Crafter){"Bakery"};



    //Receptek deklar�l�sa
    Recipe Recipes[20];

    Recipes[0] = (Recipe){.itemIds = {0, NULL}, .itemAmounts = {2,NULL}, .resultId = 7, .resultAmount = 1, .type = Windmill, .t = 30, .id = 0}; //Liszt
    Recipes[1] = (Recipe){.itemIds = {2, NULL}, .itemAmounts = {6,NULL}, .resultId = 7, .resultAmount = 4, .type = Windmill, .t = 90, .id = 1}; //Liszt
    Recipes[2] = (Recipe){.itemIds = {3, NULL}, .itemAmounts = {2,NULL}, .resultId = 9, .resultAmount = 1, .type = Windmill, .t = 60, .id = 2}; //Ketchup

    Recipes[3] = (Recipe){.itemIds = {7, NULL}, .itemAmounts = {2,NULL}, .resultId = 8, .resultAmount = 1, .type = Bakery, .t = 60, .id = 3}; //Keny�r
    Recipes[4] = (Recipe){.itemIds = {7, NULL}, .itemAmounts = {10,NULL}, .resultId = 8, .resultAmount = 6, .type = Bakery, .t = 300, .id = 4}; //Keny�r
    Recipes[5] = (Recipe){.itemIds = {7, 5}, .itemAmounts = {2,1}, .resultId = 10, .resultAmount = 1, .type = Bakery, .t = 120, .id = 5}; //Pumpkinpie

    Recipes[6] = (Recipe){.itemIds = {1, NULL}, .itemAmounts = {6,NULL}, .resultId = 11, .resultAmount = 1, .type = Brewery, .t = 240, .id = 6}; //Vodka
    Recipes[7] = (Recipe){.itemIds = {0, 1}, .itemAmounts = {6,5}, .resultId = 12, .resultAmount = 1, .type = Brewery, .t = 300, .id = 7}; //Whiskey
    Recipes[8] = (Recipe){.itemIds = {2, 1}, .itemAmounts = {10,3}, .resultId = 12, .resultAmount = 2, .type = Brewery, .t = 420, .id = 8}; //Whiskey

    int RecipeCount = 9;

    RecipesData = (ArrayData){Recipes, RecipeCount};

    //Ablak l�terhoz�sa
    CreateWindow("FarmerRealm", d_windowSizeX, d_windowSizeY, &window, &renderer);


    Init_Map(&PlantingData, &BuildingData, crafters);

    LastTile = &matrix.matrix[0][0];

    Canvas canvas;
    Init_GUI(&canvas, &PlantingData, &BuildingData);


    //A "bark�cs" �p�letekhez tartoz� men� l�trehoz�sa
    CreateCraftPanel(renderer,"Windmill", (Vector2){d_windowSizeX,d_windowSizeY},&RecipesData,Windmill,AllItem, &crafters[0].menu);
    CreateCraftPanel(renderer,"Brewery", (Vector2){d_windowSizeX,d_windowSizeY},&RecipesData,Brewery,AllItem, &crafters[1].menu);
    CreateCraftPanel(renderer,"Bakery", (Vector2){d_windowSizeX,d_windowSizeY},&RecipesData,Bakery,AllItem, &crafters[2].menu);

    crafters[0].type = Windmill;
    crafters[1].type = Brewery;
    crafters[2].type = Bakery;

    printf("GUI loaded...\n");
    printf("Canvas exist %d\n",canvas.plantPanel.childCount);

    //Ha m�r van elmentett id� akkor ez hozz�adja a mostanihoz
    unsigned plussTime = timer;

    //H�tt�rsz�n be�ll�t�sa
    SDL_SetRenderDrawColor(renderer, 3, 190, 252, 255);

    SDL_Event event;

    double iconSize = 50;
    bool grow = false;


    int SelectedTileId = 0;
    int SelectInfoAlpha= 0;

    bool LeftMouseDown;

    int warehouseIDs[ItemCount];

    int sinceLastSave = 0;

    bool BuildMode = false;
    bool PlantMode = false;


    Vector2 lastMousePos;


    Image modeSign;
    GUI_Panel selectInfo;
    CreateImage(renderer,"Images/GUI/InfoButton.png", (Vector2){0,0},(Vector2){300,90},&selectInfo.panelImage);

    //F� ciklus
    while (event.type != SDL_QUIT)
    {
        //Inputok kezel�se
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == 1)
                    {
                        LeftMouseDown = true;
                        lastMousePos = (Vector2){event.motion.x,event.motion.y};
                        OnLeftMouseDown(event);
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == 1)
                    {
                        LeftMouseDown = false;
                        //Kisz�m�tjuk az eg�rrel megtett �t hossz�t, hogy meg�llap�tsuk, hogy click volt-e vagy vonszol�s
                        if(sqrt(pow(lastMousePos.x-event.motion.x,2)+pow(lastMousePos.y-event.motion.y,2)) < 10)
                            OnLeftMouseUp(event, &canvas, AllItem, ItemCount,warehouseIDs,&PlantingData, &BuildingData, crafters, &BuildMode, &PlantMode, &RecipesData, &InventoryData);

                        if(LastTile != NULL)
                        {
                            SelectedTileId = LastTile ->id;
                            SelectInfoAlpha = 255;
                        }
                    }
                    break;
                case SDL_MOUSEMOTION:
                    OnMouseMove(event, AllItem, ItemCount, crafters, LeftMouseDown);
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_UP)
                        AddXP(level, &canvas.levelText,&canvas.levelProgression);
                    if(event.key.keysym.sym == SDLK_DOWN && level > 1)
                        AddXP(-level, &canvas.levelText,&canvas.levelProgression);
                    if(event.key.keysym.sym == SDLK_RIGHT)
                        AddMoney(1000, &canvas.moneyText);
                    if(event.key.keysym.sym == SDLK_LEFT && money > 1000)
                        AddMoney(-1000, &canvas.moneyText);
                    if(event.key.keysym.sym == SDLK_p)
                        ListProcesses(&plist);
                    if(event.key.keysym.sym == SDLK_i)
                    {
                        printf("Actual inventory:\n");
                        for(int i = 0; i<ItemCount;i++)
                            printf("\to %s: %d\n",AllItem[i].Name, AllItem[i].Amount);
                    }
                    if(event.key.keysym.sym == SDLK_t)
                        printf("Actual game time: %u\n",timer);

            }
        }

        //Id� kezel�se
        unsigned now;
        unsigned delta_time;

        now = SDL_GetTicks()+plussTime;
        delta_time = now - timer;
        timer += delta_time;


        //Biztons�gi ment�s 10 m�sodpercenk�nt
        if(sinceLastSave >= 10000)
        {
            SaveStats(xp, money, ((int)timer/1000));
            SaveInventory(AllItem, ItemCount);
            SaveTime();
            printf("Quick save....\n");
            sinceLastSave=0;
        }
        else
        {
            sinceLastSave+=delta_time;
        }

        //Fut� folyamatok ellen�rz�se

        for(int i = 0; i<plist.n;i++)
        {
            Tile *actual = GetTileByID(&matrix, plist.l[i].TileID);

            if(timer/1000 < plist.l[i].t && plist.l[i].type == Plant && !plist.l[i].done && strcmp(actual->name, "Greenhouse") != 0)
            {
                Process *p = &plist.l[i];
                char pathToPlant[200];
                ShopItem *plant = GetShopItemById(p->misc,PlantingData.ItemCount,PlantingData.Items);
                int ImageIndex = GetPlantImageId(plant,p->t);
                sprintf(pathToPlant,"Images/Plants/%s/%s%d.png",plant->name,plant->name,ImageIndex+1);

                Vector2 pos = {actual->img.destination.x,actual->img.destination.y};
                CreateImage(renderer, pathToPlant,pos,(Vector2){d_baseSize,d_baseSize}, &actual->additionalImage);
            }
            else if(timer/1000 >= plist.l[i].t && !plist.l[i].done)
            {
                if(plist.l[i].type == Plant)
                {
                    plist.l[i].done = true;
                    printf("End production...\n");

                    ShopItem *shopItem = GetShopItemById(plist.l[i].misc, PlantingData.ItemCount,PlantingData.Items);

                    Item *product = GetItemByName(shopItem->name, ItemCount,AllItem);

                    char path[100];

                    if(strcmp(actual->name, "Greenhouse") != 0)
                    {
                        sprintf(path,"Images/Plants/%s/%s4.png",shopItem->name,shopItem->name);

                        Vector2 pos = {actual->img.destination.x,actual->img.destination.y};
                        CreateImage(renderer, path,pos,(Vector2){d_baseSize,d_baseSize}, &actual->additionalImage);
                    }

                    sprintf(path,"Images/Icons/%s.png",product->Name);

                    CreateImage(renderer,path, (Vector2){100,100}, (Vector2){50,50}, &actual->icon);

                    printf("Production on %s(%d:%d):%s ended\n",actual->name,actual->coordinates.x,actual->coordinates.y,shopItem->name);
                }
                else if(plist.l[i].type == Building)
                {
                    int id = 0;

                    for(int k = 0; k<BuildingData.ItemCount+1; k++)
                    {
                        if(BuildingData.Items[k].ID == plist.l[i].misc)
                        {
                            id = k;
                        }
                    }

                    char path[100];
                    sprintf(path, "Images/%s.png",BuildingData.Items[id].name);
                    Vector2 position = {actual->img.destination.x,actual->img.destination.y};
                    strcpy(actual->name,BuildingData.Items[id].name);
                    CreateImage(renderer,path, position,(Vector2){d_baseSize,d_baseSize}, &actual->img);
                    SetSave(plist.l[i].TileID,plist.l[i].misc,0);

                    printf("Building of %s (%d) is done\n",BuildingData.Items[id].name,BuildingData.Items[id].ID);

                    Tile *tile = GetTileByID(&matrix, plist.l[i].TileID);

                    tile->process = NULL;

                    RemoveProcess(&plist, plist.l[i].ProcessID);

                    //Ha m�hkas �p�lt meg akkor elind�tja automatikusan benne a termel�st
                    if(strcmp(BuildingData.Items[id].name, "Beehive") == 0)
                    {
                        Process p = {tile->id,Loop,tile->id,timer/1000+600,6};
                        tile->process = AddProcess(&plist, p);


                        unsigned long long int misc = timer/1000+600;
                        misc <<= 8;
                        misc += 6;

                        SetSave(tile->id, 22,misc);
                        printf("Honey production started\n");
                    }
                }
                else if(plist.l[i].type == Craft)
                {
                    Recipe *recipe = &Recipes[plist.l[i].misc];
                    Item *finalItem = &AllItem[recipe->resultId];
                    printf("Crafting process done (%s)!\n", finalItem->Name);
                    AllItem[recipe->resultId].Amount += recipe->resultAmount;

                    Crafter *actualCrafter = NULL;
                    int crafterID = NULL;

                    for(int x = 0; x<3 && actualCrafter == NULL; x++)
                    {
                        if(plist.l[i].TileID == crafters[x].base->id)
                        {
                            actualCrafter = &crafters[x];
                            crafterID = x;
                        }
                    }
                    printf("\to Crafter: %s\n",actualCrafter->name);
                    int slotId = (plist.l[i].ProcessID/plist.l[i].TileID)-1;
                    printf("\to Crafter slot id: %d\n",slotId);

                    actualCrafter ->ProcessIds[slotId] = NULL;

                    RemoveProcess(&plist, plist.l[i].ProcessID);

                    int modId = d_mapSize+(crafterID*3)+slotId;

                    SetSave(modId, 0, 0);
                }
                else if(plist.l[i].type == Loop)
                {
                    plist.l[i].done = true;

                    Item *product = &AllItem[plist.l[i].misc];

                    char path[100];
                    sprintf(path,"Images/Icons/%s.png",product->Name);

                    CreateImage(renderer,path, (Vector2){100,100}, (Vector2){50,50}, &actual->icon);

                    printf("Looped production on %s(%d:%d):%s ended\n",actual->name,actual->coordinates.x,actual->coordinates.y,product->Name);
                }
            }

        }


        //Kiv�lasztott csempe adatainak megjelen�t�se
        Tile *select = GetTileByID(&matrix, SelectedTileId);
        Process *selectedProcess = GetProcess(&plist,SelectedTileId);
        char infos[100];
        char pathToIcon[100];

        if(selectedProcess != NULL)
        {
            int timeLeft = (selectedProcess->t-(timer/1000));
            char formattedTime[30];
            FormatTime(timeLeft*1000, formattedTime);


            if(selectedProcess->type == Plant)
            {
                for(int i = 0; i<PlantingData.ItemCount; i++)
                {
                    if(PlantingData.Items[i].ID == selectedProcess->misc)
                        sprintf(infos,"%s (%s)",PlantingData.Items[i].name,formattedTime);
                }
            }
            else if(selectedProcess->type == Building)
            {
                for(int i = 0; i<BuildingData.ItemCount; i++)
                {
                    if(BuildingData.Items[i].ID == selectedProcess->misc)
                        sprintf(infos,"%s (%s)",BuildingData.Items[i].name,formattedTime);
                }
            }
            else if(strcmp(select->name,"Beehive") == 0)
            {
                sprintf(infos,"Honey (%s)",formattedTime);
            }
            else
                sprintf(infos,"%s (%s)",select->name,formattedTime);
        }
        else
        {
            sprintf(infos,"%s",select->name);
            selectInfo.childCount = 1;
        }


        if(SelectInfoAlpha > 1)
        {
            SelectInfoAlpha-=delta_time/10;
            SDL_DestroyTexture(selectInfo.children[0].texture);
            int size = 30;
            size = strlen(infos) > 15 ? 25:30;
            CreateText(infos, (Color){80,52,30}, size, (Vector2){30,30}, renderer, font, &selectInfo.children[0]);
        }
        else
            SelectInfoAlpha = 0;


        SetParentAlpha(&selectInfo, SelectInfoAlpha);


        //M�d jelz� feliratok l�trehoz�sa
        if(PlantMode && modeSign.texture == NULL)
        {
            CreateText("Planting...",(Color){150, 10, 0}, 50, (Vector2){d_windowSizeX/2-100, 30}, renderer,font,&modeSign);
        }
        else if(BuildMode && modeSign.texture == NULL)
        {
            CreateText("Building...",(Color){150, 10, 0}, 50, (Vector2){d_windowSizeX/2-100, 30}, renderer,font,&modeSign);
        }
        else if((!BuildMode && !PlantMode) && modeSign.texture != NULL)
        {
            SDL_DestroyTexture(modeSign.texture);
            modeSign.texture = NULL;
        }

        SDL_RenderClear(renderer);

        //"Bark�cs" men�k megjelen�t�se
        for(int i = 0; i<3; i++)
        {
            if(crafters[i].opened)
            {
                for(int x = 0; x<3;x++)
                {
                    Vector2 pos = (Vector2){crafters[i].menu.children[2+2*x].destination.x,crafters[i].menu.children[2+2*x].destination.y};
                    if(crafters[i].ProcessIds[x] != NULL)
                    {
                        Process *proc = GetProcess(&plist, crafters[i].ProcessIds[x]);
                        unsigned long long int timeLeft = (proc->t-(timer/1000));

                        char timetext[40];
                        FormatTime(timeLeft*1000,timetext);
                        SDL_DestroyTexture(crafters[i].menu.children[2+2*x].texture);
                        CreateText(timetext, (Color){255,255,255}, 20, pos, renderer, font, &crafters[i].menu.children[2+2*x]);

                        Item *item = &AllItem[Recipes[proc->misc].resultId];

                        sprintf(pathToIcon, "Images/Icons/%s.png",item->Name);
                        pos.x -= 40;
                        pos.y -= 15;
                        CreateImage(renderer,pathToIcon, pos, (Vector2){40,40}, &crafters[i].menu.children[2+2*x+1]);

                    }
                    else
                    {
                        SDL_DestroyTexture(crafters[i].menu.children[2+2*x].texture);
                        CreateText("--Empty--", (Color){128,128,128}, 20, pos, renderer, font, &crafters[i].menu.children[2+2*x]);
                        crafters[i].menu.children[2+2*x+1].texture = NULL;
                    }
                }
            }
        }

        //Pulz�l� k�pek m�ret�nek kisz�m�t�sa
        iconSize += delta_time*0.05*(grow?1:-1);

        if((grow && iconSize > 50) || (!grow && iconSize < 30))
            grow = !grow;

        //T�rk�p elemek �s a csemp�khez tartoz� ikonok �s k�pek megjelen�t�se

        for(int x = 0; x<matrix.xSize;x++)
        {
            for(int y = 0; y<matrix.ySize;y++)
            {
                if(LastTile != NULL && LastTile -> id == matrix.matrix[x][y].id)
                    SDL_SetTextureColorMod(matrix.matrix[x][y].img.texture,220,220,220);
                else
                    SDL_SetTextureColorMod(matrix.matrix[x][y].img.texture,255,255,255);

                SDL_RenderCopy(renderer, matrix.matrix[x][y].img.texture, NULL, &matrix.matrix[x][y].img.destination);

                if(matrix.matrix[x][y].additionalImage.texture != NULL)
                {
                    SDL_RenderCopy(renderer, matrix.matrix[x][y].additionalImage.texture, NULL, &matrix.matrix[x][y].additionalImage.destination);
                }

                if(matrix.matrix[x][y].icon.texture != NULL)
                {

                    matrix.matrix[x][y].icon.destination.x = matrix.matrix[x][y].img.destination.x+75+(50-iconSize)/2;
                    matrix.matrix[x][y].icon.destination.y = matrix.matrix[x][y].img.destination.y+75+(50-iconSize)/2;


                    matrix.matrix[x][y].icon.destination.h = iconSize;
                    matrix.matrix[x][y].icon.destination.w = iconSize;

                    SDL_RenderCopy(renderer, matrix.matrix[x][y].icon.texture, NULL, &matrix.matrix[x][y].icon.destination);

                    if(matrix.matrix[x][y].arrow != NULL && ((!PlantMode && !BuildMode) || (PlantMode && matrix.matrix[x][y].process != NULL)))
                    {
                        matrix.matrix[x][y].icon.texture = NULL;
                        matrix.matrix[x][y].arrow = false;
                    }

                }
                else if(matrix.matrix[x][y].icon.texture == NULL)
                {
                    bool build =BuildMode && strcmp(matrix.matrix[x][y].name, "Ground") == 0 && BuildingData.Items[BuildingData.actualItem].ID != 10;
                    bool plant = PlantMode && matrix.matrix[x][y].process == NULL && (strcmp(matrix.matrix[x][y].name, "Greenhouse") == 0 || strcmp(matrix.matrix[x][y].name, "Field") == 0);
                    bool destroy = BuildMode && BuildingData.Items[BuildingData.actualItem].ID == 10;


                    char *Undestroyables[6] = {"Windmill","Bakery","Brewery","Shop","Warehouse","Ground"};

                    for(int i = 0; i<6; i++)
                    {
                        if(strcmp(Undestroyables[i], matrix.matrix[x][y].name) == 0)
                        {
                            destroy = false;
                        }
                    }

                    if(plant || build || destroy)
                    {
                        CreateImage(renderer, "Images/GUI/Arrow.png", (Vector2){matrix.matrix[x][y].img.destination.x,matrix.matrix[x][y].img.destination.y+10},(Vector2){150,150}, &matrix.matrix[x][y].icon);
                        matrix.matrix[x][y].arrow = true;
                    }
                }
            }
        }

        //Rakt�r �p�let megjelen�t�se ha meg van nyitva
        if(canvas.warehousePanel.visible)
        {
            RenderParent(renderer, canvas.warehousePanel);
        }

        RenderParent(renderer,selectInfo);

        RenderCanvas(&canvas, renderer);

        //Szint l�pt�k�nek kisz�mol�sa

        SDL_RenderCopy(renderer, canvas.levelProgression.texture, NULL, &canvas.levelProgression.destination);


        //M�d jelz� megjelen�t�se
        if(modeSign.texture != NULL)
            SDL_RenderCopy(renderer, modeSign.texture, NULL, &modeSign.destination);


        SDL_RenderCopy(renderer, canvas.levelText.texture, NULL, &canvas.levelText.destination);

        //Az �p�t�-�s �ltet� men� anim�lt megjelen�t�se
        ShowAnimatedGUI(renderer, &canvas.buildPanel, d_windowSizeY, BuildingData.Items, BuildingData.ChildCount,money,level);
        ShowAnimatedGUI(renderer, &canvas.plantPanel, d_windowSizeY, PlantingData.Items, PlantingData.ChildCount,money,level);


        //Rombol� gomb k�l�n ker�l megjelen�t�sre mivel ez nem hasonl�t az �ltet� panel elemeihez �gy nem szabv�nyos
        SDL_RenderCopy(renderer, canvas.buildPanel.children[canvas.buildPanel.childCount-1].texture,NULL,&canvas.buildPanel.children[canvas.buildPanel.childCount-1].destination);

        for(int i = 0; i<3; i++)
            if(crafters[i].opened)
                RenderParent(renderer, crafters[i].menu);

        SDL_RenderPresent(renderer);
    }

    //Dinamikus t�mb felszabad�t�sa
    free(plist.l);
    FreeMatrix(&matrix);

    //Adatok ment�se
    SaveStats(xp, money, ((int)timer/1000));
    SaveInventory(AllItem, ItemCount);
    printf("Saving...\n");

    //Fennmarad� dolgok bez�r�sa
    TTF_CloseFont(font);
    SDL_Quit();
    IMG_Quit();

    return 0;
}

void OnLeftMouseDown(SDL_Event e)
{
    for(int x = 0; x<matrix.xSize;x++)
    {
        for(int y = 0; y<matrix.ySize;y++)
        {
            matrix.matrix[x][y].positionToMouse = (Vector2){e.motion.x-matrix.matrix[x][y].img.destination.x,e.motion.y-matrix.matrix[x][y].img.destination.y};
        }
    }

    if(LastTile != NULL)
        SDL_SetTextureColorMod(LastTile -> img.texture,200,200,200);
}

void OnLeftMouseUp(SDL_Event e,Canvas *canvas, Item *AllItem, int ItemCount, int* ids, ShopData* plantData, ShopData* buildData, Crafter *crafters, bool *BuildMode, bool *PlantMode, ArrayData *recipeData, ArrayData *invData)
{
    LastTile = GetTileFromPosition(&matrix, (Vector2){e.motion.x,e.motion.y});

    bool clicked = false;

    Recipe *Recipes = recipeData->arr;
    int RecipeCount = recipeData->n;

    //Bark�cs folyamatok elind�t�sa

    if(OverUI((Vector2){e.motion.x,e.motion.y},  crafters[0].menu.panelImage.destination))
    {
        int indexToItem = 0;
        RecipeType recipeType;
        int crafterID = 0;
        for(int i = 0; i<3; i++)
        {
            if(crafters[i].opened)
            {
                printf("Clicked on %s panel\n", crafters[i].name);
                crafterID = i;

                for(int k = 8; k<11; k++)
                {
                    if(OverUI((Vector2){e.motion.x,e.motion.y},  crafters[i].menu.children[k].destination))
                    {
                        clicked = true;
                        recipeType = crafters[i].type;
                        indexToItem = k-8;
                    }
                }
            }
        }


        if(clicked)
        {
            int index = 0;
            Recipe *possibles[3];
            Recipe *toMake = NULL;
            for(int i = 0; i<RecipeCount; i++)
            {
                if(Recipes[i].type == recipeType)
                {
                    possibles[index] = &Recipes[i];
                    index++;
                }
            }


            toMake = possibles[indexToItem];

            if(!(AllItem[toMake->itemIds[0]].Amount >= toMake->itemAmounts[0] && (toMake->itemAmounts[1] == NULL || AllItem[toMake->itemIds[1]].Amount >= toMake->itemAmounts[1])))
            {
                printf("Not enough resources!\n");
                return;
            }

            int slotID = 0;
            bool finded = false;

            for(int i = 0; i<3 && !finded; i++)
            {
                if(crafters[crafterID].ProcessIds[i] == NULL)
                {
                    slotID = i;
                    finded = true;
                }
            }

            if(!finded)
            {
                printf("No more slot\n");
                return;
            }

            AllItem[toMake->itemIds[0]].Amount -= toMake->itemAmounts[0];

            if(toMake->itemIds[1] != NULL)
                AllItem[toMake->itemIds[1]].Amount -= toMake->itemAmounts[1];


            printf("Recipe selected: %s with index of %d to slot %d\n",AllItem[possibles[indexToItem]->resultId].Name, indexToItem, slotID);

            unsigned long long int timeToDone = (timer/1000)+toMake->t;

            Process process = {.ProcessID = crafters[crafterID].base->id*(slotID+1), .type = Craft, .TileID = crafters[crafterID].base->id, .t=timeToDone, .misc = toMake->id, .done=false};

            AddProcess(&plist, process);

            crafters[crafterID].ProcessIds[slotID] = process.ProcessID;

            int modId = d_mapSize+(crafterID*3)+slotID;

            unsigned long long int misc = timeToDone;
            misc <<= 8;
            misc += toMake->id;

            SetSave(modId, 125+crafterID, misc);

            printf("Crafting added: ID: %d; Type: %d; TileID: %d; Time: %d; Misc.: %d; Done: %s\n", process.ProcessID,process.type,process.TileID,process.t, process.misc,process.done ? "True":"False");

            return;
        }
    }

    //�p�tkez�s men� elemeinek kiv�laszt�sa

    if(canvas->buildPanel.visible && OverUI((Vector2){e.motion.x, e.motion.y}, canvas->buildPanel.panelImage.destination))
    {
        for(int i =2; i<canvas->buildPanel.childCount; i+= buildData->ChildCount)
        {
            if(OverUI((Vector2){e.motion.x, e.motion.y}, canvas->buildPanel.children[i].destination))
            {
                if(buildData->Items[(i-2)/buildData->ChildCount].price <= money && buildData->Items[(i-2)/buildData->ChildCount].level <= level)
                {
                    *BuildMode = !*BuildMode;

                    if(*BuildMode)
                    {
                        buildData->actualItem = (i-2)/6;
                        printf("Building selected: %s\n",buildData->Items[buildData->actualItem].name);
                    }
                    else
                    {
                        printf("Building cancelled\n");
                    }

                    clicked = true;
                }
                else
                {
                    *BuildMode = false;
                }
            }
        }


        if(canvas->buildPanel.visible && OverUI((Vector2){e.motion.x,e.motion.y},  canvas->buildPanel.children[0].destination))
        {
            canvas->buildPanel.visible = false;
            *BuildMode = false;
        }

        return;
    }


    //�ltet�s men� elemeinek kiv�laszt�sa
    if(canvas->plantPanel.visible && OverUI((Vector2){e.motion.x, e.motion.y}, canvas->plantPanel.panelImage.destination))
    {
        for(int i =2; i<canvas->plantPanel.childCount; i+= plantData->ChildCount)
        {
            if(OverUI((Vector2){e.motion.x, e.motion.y}, canvas->plantPanel.children[i].destination))
            {
                if(plantData->Items[(i-2)/plantData->ChildCount].price <= money && plantData->Items[(i-2)/plantData->ChildCount].level <= level)
                {
                    *PlantMode = !*PlantMode;

                    if(*PlantMode)
                    {
                        plantData->actualItem = (i-2)/6;
                        printf("Plant selected(%d): %s\n",plantData->actualItem, plantData->Items[plantData->actualItem].name);
                    }
                    else
                    {
                        printf("Planting cancelled\n");
                    }

                    clicked = true;
                }
                else
                {
                    *BuildMode = false;
                }
            }
        }

        if(canvas->plantPanel.visible && OverUI((Vector2){e.motion.x,e.motion.y},  canvas->plantPanel.children[0].destination))
        {
            canvas->plantPanel.visible = false;
            *PlantMode = false;
        }

        return;
    }


    if(LastTile != NULL)
    {
        SDL_SetTextureColorMod(LastTile -> img.texture,220,220,220);
        printf("[%d](%d:%d) - %s\n",LastTile -> id,LastTile -> coordinates.x, LastTile -> coordinates.y, LastTile -> name);
    }
    else
    {
        printf("\n\n\tLast tile is NULL\n\n\n");
        return;
    }

    //"Bark�cs" men�k megnyit�sa/bez�r�sa
    bool opened = false;
    for(int i = 0; i<3 && !canvas->warehousePanel.visible && !*BuildMode && !*PlantMode; i++)
        if(crafters[i].opened)
            opened = true;


    for(int i = 0; i<3; i++)
    {
        if(OverUI((Vector2){e.motion.x,e.motion.y}, crafters[i].menu.children[0].destination) && crafters[i].opened)
        {
            crafters[i].opened = false;
        }
        else if(strcmp(LastTile -> name, crafters[i].name) == 0 && !crafters[i].opened && !opened && !canvas->warehousePanel.visible && !*BuildMode && !*PlantMode)
        {
            crafters[i].opened = true;
            LastTile = &matrix.matrix[0][0];

            canvas->plantPanel.visible = false;
            canvas->buildPanel.visible = false;
            *BuildMode = false;
        }
    }

    if(strcmp(LastTile->name, "Warehouse") == 0)
    {
        canvas->warehousePanel.visible = true;
        OpenWarehouse(canvas, renderer, invData, font,ids);
    }

    if(canvas->warehousePanel.visible && OverUI((Vector2){e.motion.x,e.motion.y},  canvas->warehousePanel.panelImage.destination))
    {
        if(OverUI((Vector2){e.motion.x,e.motion.y},  canvas->warehousePanel.children[0].destination))
        {
            canvas->warehousePanel.visible = false;
        }

        int index = 0;
        for(int i = 5; i<canvas->warehousePanel.childCount && !clicked; i+=5)
        {
            if(OverUI((Vector2){e.motion.x,e.motion.y},  canvas->warehousePanel.children[i].destination))
            {
                clicked = true;
                AllItem[ids[index]].Amount--;

                AddMoney(AllItem[ids[index]].Price, &canvas->moneyText);
                OpenWarehouse(canvas, renderer, invData, font,ids);
            }
            index++;
        }
    }


    if(!canvas->buildPanel.visible && OverUI((Vector2){e.motion.x,e.motion.y},  canvas->buildButton.destination))
    {
        canvas->buildPanel.visible = true;
    }
    if(!canvas->plantPanel.visible && OverUI((Vector2){e.motion.x,e.motion.y},  canvas->plantButton.destination))
    {
        canvas->plantPanel.visible = true;
    }

    if(*BuildMode && (strcmp(LastTile -> name, "Ground") == 0 || strcmp(buildData->Items[buildData->actualItem].name, "Ground") == 0))
    {
        char *Undestroyables[6] = {"Windmill","Bakery","Brewery","Shop","Warehouse","Ground"};

        for(int i = 0; i<6 && strcmp(buildData->Items[buildData->actualItem].name, "Ground") == 0;i++)
        {
            if(strcmp(LastTile->name, Undestroyables[i]) == 0)
                return;
        }

        AddMoney(-buildData->Items[buildData->actualItem].price, &canvas->moneyText);

        unsigned int id = LastTile->id;
        unsigned long long int t = (timer/1000)+buildData->Items[buildData->actualItem].time;

        Process p = {id,Building,id,t,buildData->Items[buildData->actualItem].ID};
        LastTile->process = AddProcess(&plist,p);

        Vector2 position = {LastTile->img.destination.x,LastTile->img.destination.y};

        CreateImage(renderer,"Images/ConstructionSite.png", position,(Vector2){d_baseSize,d_baseSize}, &LastTile->img);
        strcpy(LastTile->name,"ConstructionSite");


        *BuildMode = false;


        unsigned long long int misc = t;
        misc <<= 8;
        misc += buildData->Items[buildData->actualItem].ID;

        SetSave(id,1,misc);

        printf("%s(%d) building started (%d sec)\n",buildData->Items[buildData->actualItem].name,buildData->Items[buildData->actualItem].ID,buildData->Items[buildData->actualItem].time);
    }

    if(*PlantMode && (strcmp(LastTile -> name, "Field") == 0 || strcmp(LastTile -> name, "Greenhouse") == 0) && LastTile->process == NULL)
    {
        bool greenhouse = strcmp(LastTile->name,"Greenhouse") == 0;
        AddMoney(-plantData->Items[plantData->actualItem].price, &canvas->moneyText);

        unsigned int id = LastTile->id;
        unsigned long long int t = (timer/1000)+(plantData->Items[plantData->actualItem].time/(greenhouse?3:1));

        Process p = {id,Plant,id,t,plantData->Items[plantData->actualItem].ID};
        LastTile->process = AddProcess(&plist,p);

        unsigned long long int misc = t;
        misc <<= 8;
        misc += plantData->Items[plantData->actualItem].ID;

        SetSave(id,strcmp(LastTile->name,"Field") == 0 ? 21:23,misc);

        if(plantData->Items[plantData->actualItem].price > money)
            *PlantMode = false;

        printf("%s placed on field (%d:%d)\n",plantData->Items[plantData->actualItem].name, LastTile ->coordinates.x, LastTile ->coordinates.y);

        return;
    }

    Process *selectedProcess = GetProcess(&plist, LastTile->id);
    if(selectedProcess != NULL && selectedProcess->t < timer/1000)
    {
        Item *item;
        Tile *actual = GetTileByID(&matrix, selectedProcess->TileID);
        if(selectedProcess->type == Plant)
        {
            ShopItem *plant = GetShopItemById(selectedProcess->misc,plantData->ItemCount, plantData->Items);
            printf("Product (%s) is done\n",plant->name);
            item = GetItemByName(plant->name, ItemCount, AllItem);
            item->Amount++;

            SetSave(selectedProcess->TileID, strcmp(LastTile->name,"Field") == 0 ? 21:23,0);

            RemoveProcess(&plist,selectedProcess->ProcessID);
            LastTile->process = NULL;
        }
        else if(selectedProcess->type == Loop)
        {
            item = &AllItem[selectedProcess->misc];
            item->Amount++;
            printf("Product (%s) is done\n",item->Name);
            RemoveProcess(&plist,selectedProcess->ProcessID);

            long long int timeToDone = timer/1000+600;

            printf("Beehive looped production started (%d)\n",timeToDone);
            Process p = {actual ->id,Loop,actual ->id,timeToDone,6};
            actual -> process = AddProcess(&plist, p);


            unsigned long long int misc = timeToDone;
            misc <<= 8;
            misc += 6;

            SetSave(actual->id, 22,misc);
        }

        actual->additionalImage.texture = NULL;
        actual->icon.texture = NULL;


        AddXP(item->Price, &canvas->levelText,&canvas->levelProgression);
        return;
    }


}

void OnMouseMove(SDL_Event e,Item *AllItem, int ItemCount, Crafter *crafters, bool LeftMouseDown)
{
    //Ha b�rmilyen panel meg van nyitva akkor kil�p

    for(int i = 0; i<3 ; i++)
        if(crafters[i].opened)
            return;


    //T�rk�p mozgat�sa

    for(int x = 0; x<matrix.xSize && LeftMouseDown;x++)
    {
        for(int y = 0; y<matrix.ySize;y++)
        {
            matrix.matrix[x][y].img.destination.x = (e.motion.x-matrix.matrix[x][y].positionToMouse.x);
            matrix.matrix[x][y].img.destination.y = (e.motion.y-matrix.matrix[x][y].positionToMouse.y);
            matrix.matrix[x][y].additionalImage.destination.x = (e.motion.x-matrix.matrix[x][y].positionToMouse.x);
            matrix.matrix[x][y].additionalImage.destination.y = (e.motion.y-matrix.matrix[x][y].positionToMouse.y);
        }
    }
}

//Az aktu�lis n�v�ny k�p�nek kisz�mol�sa a h�tral�v� id�b�l
int GetPlantImageId(ShopItem *plant, unsigned long long int TimeWhenDone)
{
    unsigned long long TimeLeft = TimeWhenDone-(timer/1000);
    unsigned int AllTime = plant->time;
    unsigned short int PictureTime = AllTime/3;


    return (AllTime-TimeLeft)/PictureTime;
}


ShopItem* GetShopItemById(int id, int n, ShopItem *itemList)
{
    for(int i = 0; i<n; i++)
    {
        if(itemList[i].ID == id)
            return &itemList[i];
    }
    return NULL;
}

void AddMoney(int amount, Image *moneyText)
{
    money += amount;
    char intToString[20];

    sprintf(intToString, "$%d", money);

    if(moneyText->texture != NULL)
        SDL_DestroyTexture(moneyText->texture);

    CreateText(intToString, (Color){80,52,30}, 50, (Vector2){880,23}, renderer, font, moneyText);
}

void AddXP(int amount, Image *levelText, Image *levelProg)
{
    printf("Added xp\n");
    xp+=amount;
    printf("All xp=%d\n",xp);
    int nextLimit = 10;
    int remainXP = xp;

    for(level = 1; remainXP >= nextLimit; level++)
    {
        remainXP-= nextLimit;
        nextLimit += (level-1)*10;
    }

    char intToString[20];
    sprintf(intToString, "Level %d", level);

    if(levelText->texture != NULL)
        SDL_DestroyTexture(levelText->texture);

    levelProg -> destination.w = 215.0*(double)remainXP/nextLimit;
    CreateText(intToString, (Color){80,52,30}, 30, (Vector2){880,85}, renderer, font, levelText);
}
