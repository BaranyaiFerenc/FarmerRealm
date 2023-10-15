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

    CreateText("--Empty--", (Color){128,128,128}, (Vector2){70,20}, (Vector2){360,307}, renderer, Font, &panel -> children[panel -> childCount]);
    panel -> childCount++;
    CreateText("--Empty--", (Color){128,128,128}, (Vector2){70,20}, (Vector2){360,377}, renderer, Font, &panel -> children[panel -> childCount]);
    panel -> childCount++;
    CreateText("--Empty--", (Color){128,128,128}, (Vector2){70,20}, (Vector2){360,447}, renderer, Font, &panel -> children[panel -> childCount]);
    panel -> childCount++;
}

int GetTextLength(char text[])
{
    int i = 0;

    for(i; text[i] != '\0'; i++);

    return i;
}
