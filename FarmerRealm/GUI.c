#include "GUI.h"

void CreateText(char const text[], Color color, Vector2 sizeOfText, Vector2 position, SDL_Renderer* renderer, TTF_Font *font, Image *img)
{
    int length = GetTextLength(text);
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text, (SDL_Color){color.r,color.g,color.b}, sizeOfText.y*length);


    img -> texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    img -> destination.w = sizeOfText.x;
    img -> destination.h = sizeOfText.y;

    img -> destination.x = position.x;
    img -> destination.y = position.y;
}

void RenderParent(SDL_Renderer* renderer, GUI_Panel parent)
{
    SDL_RenderCopy(renderer, parent.panelImage.texture, NULL, &parent.panelImage.destination);
    int index = 0;
    while(index < parent.childCount)
    {
        SDL_RenderCopy(renderer, parent.children[index].texture, NULL, &parent.children[index].destination);
        index++;
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

void CreateCraftPanel(SDL_Renderer* renderer,char const title[], Vector2 windowSize, GUI_Panel *panel)
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

    CreateText(title, (Color){80,52,30}, (Vector2){150,50}, titlePos, renderer, Font, &panel -> children[panel -> childCount]);
    panel -> childCount++;

    CreateText("--Empty--", (Color){128,128,128}, (Vector2){70,20}, (Vector2){panelPosition.x+60,panelPosition.y+108}, renderer, Font, &panel -> children[panel -> childCount]);
    panel -> childCount++;
    CreateText("--Empty--", (Color){128,128,128}, (Vector2){70,20}, (Vector2){panelPosition.x+60,panelPosition.y+178}, renderer, Font, &panel -> children[panel -> childCount]);
    panel -> childCount++;
    CreateText("--Empty--", (Color){128,128,128}, (Vector2){70,20}, (Vector2){panelPosition.x+60,panelPosition.y+248}, renderer, Font, &panel -> children[panel -> childCount]);
    panel -> childCount++;
}

int GetTextLength(char text[])
{
    int i = 0;

    for(i; text[i] != '\0'; i++);

    return i;
}

void ShowAnimatedGUI(SDL_Renderer* renderer, GUI_Panel *panel, int PanelSpeed, int windowSizeY)
{
    if(panel -> visible && panel -> panelImage.destination.y == windowSizeY-panel -> panelImage.destination.h)
    {
        RenderParent(renderer, *panel);
    }
    else if(!panel -> visible && panel -> panelImage.destination.y < windowSizeY)
    {
        panel -> panelImage.destination.y += PanelSpeed;

        for(int i = 0; i<panel -> childCount; i++)
        {
            panel -> children[i].destination.y += PanelSpeed;
        }

        RenderParent(renderer, *panel);
    }
    else if(panel -> visible && panel -> panelImage.destination.y >= windowSizeY-panel -> panelImage.destination.h)
    {
        panel -> panelImage.destination.y -= PanelSpeed;

        for(int i = 0; i<panel -> childCount; i++)
        {
            panel -> children[i].destination.y -= PanelSpeed;
        }
        RenderParent(renderer, *panel);
    }
}

void CheckShopItems(GUI_Panel *parent, ShopItem *items, int childcount ,int money, int level)
{
    for(int i = 5; i<parent -> childCount && parent -> visible; i+=childcount)
    {
        unsigned short int index = (i-5)/childcount;
        if(money >= items[index].price && level >= items[index].level)
        {
            SDL_SetTextureColorMod(parent -> children[i+1].texture,100,255,100); //Ár
            SDL_SetTextureColorMod(parent -> children[i+2].texture,255,255,255); //Szint
            SDL_SetTextureColorMod(parent -> children[i-3].texture,255,255,255); //Háttér

            SDL_SetTextureAlphaMod(parent -> children[i-1].texture, 0);
        }
        else if(level < items[index].level)
        {
            SDL_SetTextureColorMod(parent -> children[i].texture,220,220,220);
            SDL_SetTextureColorMod(parent -> children[i+2].texture,220,100,100);
            SDL_SetTextureColorMod(parent -> children[i-3].texture,220,220,220);

            SDL_SetTextureAlphaMod(parent -> children[i-1].texture, 255);
        }
        else
        {
            SDL_SetTextureColorMod(parent -> children[i+1].texture,255,100,100);
            SDL_SetTextureColorMod(parent -> children[i+2].texture,255,255,255);
            SDL_SetTextureColorMod(parent -> children[i-3].texture,255,220,220);

            SDL_SetTextureAlphaMod(parent -> children[i-1].texture, 0);
        }
    }
}


void FormatTime(int t, char *out)
{
    int hour = t/1000/60/60;
    int minute = (t/1000/60)-(hour*60);
    int sec = (t/1000)-(hour*60*60)-(minute*60);

    sprintf(out, "%02d:%02d:%02d", hour,minute,sec);
}
