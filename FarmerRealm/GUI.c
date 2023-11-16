#include "GUI.h"


#define gPanelSpeed 30

void CreateText(char text[], Color color, int sizeOfText, Vector2 position, SDL_Renderer* renderer, TTF_Font *font, Image *img)
{
    int length = strlen(text);
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text, (SDL_Color){color.r,color.g,color.b}, length*sizeOfText);

    img -> texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    img -> destination.w = (sizeOfText/2)*length;
    img -> destination.h = sizeOfText;

    img -> destination.x = position.x;
    img -> destination.y = position.y;
}

void RenderParent(SDL_Renderer* renderer, GUI_Panel parent)
{
    SDL_RenderCopy(renderer, parent.panelImage.texture, NULL, &parent.panelImage.destination);
    int index = 0;
    while(index < parent.childCount)
    {
        SDL_SetTextureColorMod(parent.children[index].texture,255,255,255);
        SDL_RenderCopy(renderer, parent.children[index].texture, NULL, &parent.children[index].destination);
        index++;
    }
}

void RenderShopPanel(SDL_Renderer* renderer, GUI_Panel *panel,ShopItem *items, int childcount ,int money, int level)
{
    SDL_RenderCopy(renderer, panel->panelImage.texture, NULL, &panel->panelImage.destination);

    for(int i = 0; i<5;i++)
    {
        SDL_RenderCopy(renderer, panel->children[i].texture, NULL, &panel->children[i].destination);
    }

    for(int i = 5; i<panel -> childCount && panel -> visible; i+=childcount)
    {
        unsigned short int index = (i-5)/childcount;
        if(money >= items[index].price && level >= items[index].level)
        {
            SDL_SetTextureColorMod(panel -> children[i+1].texture,100,255,100); //Ár
            SDL_SetTextureColorMod(panel -> children[i+2].texture,255,255,255); //Szint
            SDL_SetTextureColorMod(panel -> children[i-3].texture,255,255,255); //Háttér

            SDL_SetTextureAlphaMod(panel -> children[i-1].texture, 0);
        }
        else if(level < items[index].level)
        {
            SDL_SetTextureColorMod(panel -> children[i].texture,220,220,220);
            SDL_SetTextureColorMod(panel -> children[i+2].texture,220,100,100);
            SDL_SetTextureColorMod(panel -> children[i-3].texture,220,220,220);

            SDL_SetTextureAlphaMod(panel -> children[i-1].texture, 255);
        }
        else
        {
            SDL_SetTextureColorMod(panel -> children[i+1].texture,255,100,100);
            SDL_SetTextureColorMod(panel -> children[i+2].texture,255,255,255);
            SDL_SetTextureColorMod(panel -> children[i-3].texture,255,220,220);

            SDL_SetTextureAlphaMod(panel -> children[i-1].texture, 0);
        }

        SDL_RenderCopy(renderer, panel->children[i-3].texture, NULL, &panel->children[i-3].destination);
        SDL_RenderCopy(renderer, panel->children[i+1].texture, NULL, &panel->children[i+1].destination);
        SDL_RenderCopy(renderer, panel->children[i+2].texture, NULL, &panel->children[i+2].destination);
        SDL_RenderCopy(renderer, panel->children[i-2].texture, NULL, &panel->children[i-2].destination);
        SDL_RenderCopy(renderer, panel->children[i].texture, NULL, &panel->children[i].destination);
        SDL_RenderCopy(renderer, panel->children[i-1].texture, NULL, &panel->children[i-1].destination);
    }
}

Vector2 GetUpRightCornerPosition(Vector2 parentPos, Vector2 parentSize, Vector2 objSize)
{
    int x = parentPos.x+parentSize.x-objSize.x;
    int y = parentPos.y;

    return (Vector2){x,y};
}
Vector2 GetUpLeftCornerPosition(Vector2 parentPos, Vector2 parentSize, Vector2 objSize)
{
    int x = parentPos.x;
    int y = parentPos.y;

    return (Vector2){x,y};
}

bool OverUI(Vector2 mousePos, SDL_Rect obj)
{
    return (mousePos.x > obj.x && mousePos.x < obj.x+obj.w) && (mousePos.y > obj.y && mousePos.y < obj.y+obj.h);
}

void CreateCraftPanel(SDL_Renderer* renderer,char *title, Vector2 windowSize, Recipe *recipes, int recipeCount, RecipeType type, Item *invetory, int inventoryCount, GUI_Panel *panel, int *recipeAmount)
{
    TTF_Font *Font = TTF_OpenFont("system.fon", 30);
    Vector2 panelPosition = (Vector2){windowSize.x/2-200,windowSize.y/2-150};
    Vector2 panelSize = (Vector2){400,300};

    CreateImage(renderer,"Images/GUI/CraftPanel.png",panelPosition,panelSize, &panel -> panelImage);

    CreateImage(renderer,"Images/GUI/Exit.png",GetUpRightCornerPosition(panelPosition,panelSize,(Vector2){50,50}),(Vector2){50,50}, &panel -> children[panel -> childCount]);
    panel -> childCount++;

    Vector2 titlePos = GetUpLeftCornerPosition(panelPosition,panelSize,(Vector2){200,75});
    titlePos.x = titlePos.x+30;
    titlePos.y = titlePos.y+15;

    CreateText(title, (Color){80,52,30}, 50, titlePos, renderer, Font, &panel -> children[panel -> childCount]);
    panel -> childCount++;

    CreateText("--Empty--", (Color){128,128,128}, 20, (Vector2){panelPosition.x+60,panelPosition.y+108}, renderer, Font, &panel -> children[panel -> childCount]);
    panel -> childCount++;
    panel->children[panel->childCount].texture = NULL; //Majd ide fog betöltődni az ikon
    panel ->childCount++;
    CreateText("--Empty--", (Color){128,128,128}, 20, (Vector2){panelPosition.x+60,panelPosition.y+178}, renderer, Font, &panel -> children[panel -> childCount]);
    panel -> childCount++;
    panel->children[panel->childCount].texture = NULL; //Majd ide fog betöltődni az ikon
    panel ->childCount++;
    CreateText("--Empty--", (Color){128,128,128}, 20, (Vector2){panelPosition.x+60,panelPosition.y+248}, renderer, Font, &panel -> children[panel -> childCount]);
    panel -> childCount++;
    panel->children[panel->childCount].texture = NULL; //Majd ide fog betöltődni az ikon
    panel ->childCount++;

    int counter = 0;
    for(int i = 0; i<recipeCount; i++)
    {
        if(recipes[i].type == type)
        {
            Vector2 position = (Vector2){panelPosition.x+195,panelPosition.y+80+counter*70};
            CreateImage(renderer, "Images/GUI/HButton.png", position, (Vector2){200,75}, &panel -> children[panel -> childCount]);
            panel -> childCount++;


            counter++;
        }
    }

    *recipeAmount = counter;
    counter = 0;

    for(int i = 0; i<recipeCount;i++)
    {
        if(recipes[i].type == type)
        {
            Vector2 position = (Vector2){panelPosition.x+195,panelPosition.y+80+counter*70};

            position.y += 10;
            position.x += 20;
            char iconPath[50];

            Item *item = &invetory[recipes[i].itemIds[0]];

            sprintf(iconPath,"Images/Icons/%s.png",item->Name);
            CreateImage(renderer, iconPath, position, (Vector2){35,35}, &panel -> children[panel -> childCount]);
            panel -> childCount++;

            position.x+=10;
            position.y+=35;
            char num[5];
            sprintf(num, "%d",recipes[i].itemAmounts[0]);
            CreateText(num, (Color){255,255,255}, 20, position, renderer, Font, &panel -> children[panel -> childCount]);
            panel -> childCount++;


            position.x-=10;
            position.y-=35;

            if(recipes[i].itemIds[1] != NULL)
            {
                position.x += 45;
                position.y += 10;

                CreateImage(renderer, "Images/GUI/Pluss.png", position, (Vector2){20,20}, &panel -> children[panel -> childCount]);
                panel -> childCount++;

                position.x += 25;
                position.y -= 10;
                item = &invetory[recipes[i].itemIds[1]];

                sprintf(iconPath,"Images/Icons/%s.png",item->Name);
                CreateImage(renderer, iconPath, position, (Vector2){35,35}, &panel -> children[panel -> childCount]);
                panel -> childCount++;


                position.x+=10;
                position.y+=35;

                sprintf(num, "%d",recipes[i].itemIds[1]);
                CreateText(num, (Color){255,255,255}, 20, position, renderer, Font, &panel -> children[panel -> childCount]);
                panel -> childCount++;

                position.x-=10;
                position.y-=35;
            }

            position.x += 40;
            position.y += 10;

            CreateImage(renderer, "Images/GUI/ArrowR.png", position, (Vector2){20,20}, &panel -> children[panel -> childCount]);
            panel -> childCount++;

            position.x += 20;
            position.y -= 10;

            item = &invetory[recipes[i].resultId];

            sprintf(iconPath,"Images/Icons/%s.png",item->Name);
            CreateImage(renderer, iconPath, position, (Vector2){35,35}, &panel -> children[panel -> childCount]);
            panel -> childCount++;


            position.x+=10;
            position.y+=35;

            sprintf(num, "%d",recipes[i].resultAmount);
            CreateText(num, (Color){255,255,255}, 20, position, renderer, Font, &panel -> children[panel -> childCount]);
            panel -> childCount++;

            counter++;
        }
    }
}

void ShowAnimatedGUI(SDL_Renderer* renderer, GUI_Panel *panel, int windowSizeY, ShopItem *items, int childcount ,int money, int level)
{
    if(panel -> visible && panel -> panelImage.destination.y == windowSizeY-panel -> panelImage.destination.h)
    {
        //RenderParent(renderer, *panel);
        RenderShopPanel(renderer,panel,items,childcount,money,level);
    }
    else if(!panel -> visible && panel -> panelImage.destination.y < windowSizeY)
    {
        panel -> panelImage.destination.y += gPanelSpeed;

        for(int i = 0; i<panel -> childCount; i++)
        {
            panel -> children[i].destination.y += gPanelSpeed;
        }


        RenderShopPanel(renderer,panel,items,childcount,money,level);
    }
    else if(panel -> visible && panel -> panelImage.destination.y >= windowSizeY-panel -> panelImage.destination.h)
    {
        panel -> panelImage.destination.y -= gPanelSpeed;

        for(int i = 0; i<panel -> childCount; i++)
        {
            panel -> children[i].destination.y -= gPanelSpeed;
        }

        RenderShopPanel(renderer,panel,items,childcount,money,level);
    }


}

void CheckShopItems(GUI_Panel *parent, ShopItem *items, int childcount ,int money, int level)
{
}


void FormatTime(int t, char *out)
{
    int hour = t/1000/60/60;
    int minute = (t/1000/60)-(hour*60);
    int sec = (t/1000)-(hour*60*60)-(minute*60);

    sprintf(out, "%02d:%02d:%02d", hour,minute,sec);
}

void RenderCanvas(Canvas *canvas, SDL_Renderer *renderer)
{
    SDL_RenderCopy(renderer, canvas->infoBox.texture, NULL, &canvas->infoBox.destination);
    SDL_RenderCopy(renderer, canvas->moneyText.texture, NULL, &canvas->moneyText.destination);
    SDL_RenderCopy(renderer, canvas->levelText.texture, NULL, &canvas->levelText.destination);

    SDL_RenderCopy(renderer, canvas->buildButton.texture, NULL, &canvas->buildButton.destination);
    SDL_RenderCopy(renderer, canvas->plantButton.texture, NULL, &canvas->plantButton.destination);
}

void OpenWarehouse(Canvas *canvas, SDL_Renderer *renderer, Item *inventory, int n, TTF_Font *font, int *ids)
{
    if(canvas->warehousePanel.visible)
    {
        canvas->warehousePanel.childCount = 2;
        int i = 0;
        for(int k = 0; k<n; k++)
        {
            if(inventory[k].Amount > 0)
            {
                char path[100];
                sprintf(path,"Images/Icons/%s.png",inventory[k].Name);
                Vector2 position = {125+(i-5*(i/5))*130, 225+(i/5)*150};
                CreateImage(renderer,"Images/GUI/Item.png",position,(Vector2){130,150},&canvas->warehousePanel.children[canvas->warehousePanel.childCount]);
                canvas->warehousePanel.childCount++;

                position.x+= 35;
                position.y += 10;
                CreateImage(renderer,path,position,(Vector2){60,60},&canvas->warehousePanel.children[canvas->warehousePanel.childCount]);
                canvas->warehousePanel.childCount++;
                sprintf(path,"x%d",inventory[k].Amount);
                position.x+= 10;
                position.y+= 60;
                CreateText(path, (Color){80,52,30}, 30, position, renderer, font,&canvas->warehousePanel.children[canvas->warehousePanel.childCount]);
                canvas->warehousePanel.childCount++;
                position.x-=30;
                position.y+=30;
                CreateImage(renderer,"Images/GUI/Sell.png",position,(Vector2){100,40},&canvas->warehousePanel.children[canvas->warehousePanel.childCount]);
                canvas->warehousePanel.childCount++;
                position.x+= 20;
                position.y+=10;
                sprintf(path,"$%d",inventory[k].Price);
                CreateText(path, (Color){214, 79, 79}, 20, position, renderer, font,&canvas->warehousePanel.children[canvas->warehousePanel.childCount]);
                canvas->warehousePanel.childCount++;
                ids[i] = k;
                i++;
            }
        }

        if(i == 0)
        {
            CreateText("--EMPTY--", (Color){115, 88, 68}, 40, (Vector2){350,350}, renderer, font,&canvas->warehousePanel.children[canvas->warehousePanel.childCount]);
            canvas->warehousePanel.childCount++;
        }

    }
}

void SetParentAlpha(GUI_Panel *parent, int a)
{
    SDL_SetTextureAlphaMod(parent->panelImage.texture, a);

    for(int i = 0; i<parent->childCount;i++)
    {
        SDL_SetTextureAlphaMod(parent->children[i].texture, a);
    }
}
