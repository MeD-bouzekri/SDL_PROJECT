#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "functions.h"

struct Node {
    int data;
    struct Node* left;
    struct Node* right;
};
struct Node* createNode(int value) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node*));
    newNode->data = value;
    newNode->left = newNode->right = NULL;
    return newNode;
}
struct Node* insertNode(struct Node* root, int value) {
    if (root == NULL) {
        return createNode(value);
    }

    // Si la valeur est inférieure, elle va à gauche
    if (value < root->data) {
        root->left = insertNode(root->left, value);
    }
    // Si la valeur est supérieure, elle va à droite
    else if (value > root->data) {
        root->right = insertNode(root->right, value);
    }

    return root;
}
struct Node* searchNode(struct Node* root, int value) {
    if (root == NULL || root->data == value) {
        return root;
    }

    // Si la valeur est inférieure, recherche à gauche
    if (value < root->data) {
        return searchNode(root->left, value);
    }

    // Si la valeur est supérieure, recherche à droite
    return searchNode(root->right, value);
}
struct Node* findMinNode(struct Node* node) {
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}
void convertToChar5(int number, char result[5]) {
    snprintf(result, 6, "%d", number);
}
void inOrderTraversalHelper(struct Node* root, char textBuffer[6]) {
    if (root != NULL) {
        inOrderTraversalHelper(root->left, textBuffer);
        char tempBuffer[6];  // To account for the null terminator
        convertToChar5(root->data, tempBuffer);
        strcat(textBuffer, tempBuffer);
        inOrderTraversalHelper(root->right, textBuffer);
    }
}

void inOrderTraversal(struct Node* root, int *c, char textBuffer[6]) {
    strcpy(textBuffer, "");  // Initialize textBuffer
    *c = 1;
    inOrderTraversalHelper(root, textBuffer);
}
struct Node* deleteNode(struct Node* root, int value) {
    if (root == NULL) {
        return root;
    }

    // Recherche du nœud à supprimer
    if (value < root->data) {
        root->left = deleteNode(root->left, value);
    } else if (value > root->data) {
        root->right = deleteNode(root->right, value);
    } else {
        // Nœud trouvé, effectuer la suppression

        // Cas 1: Nœud avec un seul enfant ou pas d'enfant
        if (root->left == NULL) {
            struct Node* temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            struct Node* temp = root->left;
            free(root);
            return temp;
        }

        // Cas 2: Nœud avec deux enfants
        struct Node* temp = findMinNode(root->right);
        root->data = temp->data;
        root->right = deleteNode(root->right, temp->data);
    }

    return root;
}


SDL_Window *window;
SDL_Renderer *rendrer;
char textBuffer[20];
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y ,SDL_Surface *s) {
    SDL_Color textColor = {0, 0, 255}; 

    SDL_Surface* numsurface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, numsurface);

    SDL_Rect textRect;
    textRect.x = x;
    textRect.y = y;
    textRect.w = numsurface->w;
    textRect.h = numsurface->h;

    
    SDL_BlitSurface(numsurface, NULL,s,&textRect);
    
    
    
}
bool isInteger(const char* string) {
    if (*string == '\0') {
        return false;  // Empty string
    }

    if (string[0] == '-') {
        return true;  // String starts with a minus sign
    }

    char* endptr;
    strtol(string, &endptr, 10);

    // Check if the entire string was converted and no non-digit characters follow
    return *endptr == '\0';
}
//Recursively draws the binary tree
void drawCurvedLine(SDL_Surface* surface, int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    Uint32 pixelColor = SDL_MapRGBA(surface->format, r, g, b, a);

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    int err2;

    while (1) {
        Uint32* pixel = (Uint32*)((Uint8*)surface->pixels + y1 * surface->pitch + x1 * sizeof(Uint32));
        *pixel = pixelColor;

        if (x1 == x2 && y1 == y2) {
            break;
        }

        err2 = 2 * err;

        if (err2 > -dy) {
            err -= dy;
            x1 += sx;
        }

        if (err2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void drawBinaryTree(SDL_Renderer* renderer, struct Node* root, int x, int y, int level, int offsetX, int numberToHighlight, int Highlight, SDL_Surface *s) {
    if (root == NULL) {
        return;
    }

    // Draw connections to left child with curved lines
    if (root->left != NULL) {
        int leftX = x - offsetX+20;
        int leftY = y + 80;
        drawCurvedLine(s, x, y +40, leftX, leftY+40, 0, 0, 0, 255);
        drawBinaryTree(renderer, root->left, leftX, leftY, level + 1, offsetX / 2 + 30, numberToHighlight, Highlight, s);
    }

    // Draw connections to right child with curved lines
    if (root->right != NULL) {
        int rightX = x + offsetX+20;
        int rightY = y +80;
        drawCurvedLine(s, x, y +40, rightX, rightY+40, 0, 0,0, 255);
        drawBinaryTree(renderer, root->right, rightX, rightY, level + 1, offsetX / 2 + 30, numberToHighlight, Highlight, s);
    }

    // Draw the current node
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    char buffer[6];
    sprintf(buffer, "%d", root->data);

    // Use SDL_ttf to display the text
    SDL_Color textColor;
    if (Highlight == 1) {
        textColor = (SDL_Color){255, 255, 255, 255}; // Set color for text

        if (root->data == numberToHighlight) {
            textColor = (SDL_Color){39, 56, 70, 255}; // Change color for the highlighted number
        }
    } else {
        textColor = (SDL_Color){39, 56, 70, 255};
    }

    TTF_Font* font = TTF_OpenFont("fonts/ARIALBD.TTF", 40);  // Open the font with size 40
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, buffer, textColor);
    SDL_Rect textRect = {x - 10, y + 40, 20, 20};

    SDL_BlitSurface(textSurface, NULL, s, &textRect);
}


void drawBinaryTreen(SDL_Renderer* renderer, struct Node* root, int x, int y,int n, int level, int offsetX, int numberToHighlight, int Highlight ,SDL_Surface *s,bool so) {
    if (root == NULL) {
        return;
    }
    int f;
    if (so==true){f=0;}
    
    // Draw the current node
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, x, y, x, y + 40);
    SDL_RenderDrawLine(renderer, x - 20, y + 40, x + 20, y + 40);
    char buffer[6];
    sprintf(buffer, "%d", root->data);

    // Use SDL_ttf to display the text
    SDL_Color textColor;
    if (Highlight == 1) {
        if (so==true && f==0 && root->data==n){textColor = (SDL_Color){0, 255, 0, 255};f=f+1;so=false;}
            else  {textColor= (SDL_Color){255, 255, 255, 255};}  // Set color for text
        
        if (root->data == numberToHighlight ) {
            
            
            if (so==true && f==0 && root->data==n){
                textColor = (SDL_Color){0, 255, 0, 255};f=f+1;so=false;
            }else {textColor = (SDL_Color){39, 56, 70, 255}; // Change color for the highlighted number
            }
            

        }
    } else {
        if (so==true && f==0 && root->data==n){
            textColor = (SDL_Color){0, 255, 0, 255};f=f+1;so=false;}
        else {textColor = (SDL_Color){39, 56, 70, 255};}
    }

    // Use SDL_ttf to display the text
    TTF_Font* font = TTF_OpenFont("fonts/ARIALI.TTF", 40);  // Open the font with size 20
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, buffer, textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {x - 10, y + 40, 20, 20};



    // Draw connections to left child
    if (root->left != NULL) {
        int leftX = x - offsetX;
        int leftY = y + 100;
        SDL_RenderDrawLine(renderer, x, y + 60, leftX, leftY);
        drawBinaryTreen(renderer, root->left, leftX, leftY,n, level + 1, offsetX / 2 + 30, numberToHighlight, Highlight,s,so);
    }

    // Draw connections to right child
    if (root->right != NULL) {
        int rightX = x + offsetX;
        int rightY = y + 100;
        SDL_RenderDrawLine(renderer, x, y + 60, rightX, rightY);
        drawBinaryTreen(renderer, root->right, rightX, rightY,n, level + 1, offsetX / 2 + 30, numberToHighlight, Highlight,s,so);
    }
    SDL_BlitSurface(textSurface, NULL,s,&textRect);
    
}
void DEL(char *str, int index) {
    int len = strlen(str);
    memmove(&str[index], &str[index + 1], len - index);
}
int main(int argc , char *argv[]){
    int RUNNING,x,y,l,l1,c;
    bool k=false;bool handleMouseMotion = true;bool s=false;
    struct Node* root=NULL;struct Node* foundNode;
    SDL_Rect startButtonRect, exitButtonRect,insereButtonRect,recherchButtonRect,affichButtonRect,supprimButtonRect,quitButtonRect,idkButtonRect,inputrect;
    SDL_Rect center; 
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface *background ,*png_image,*gScreenSurface,*png_image1,*png_image2,*png_image3;
    //SDL_Rect cursorRegion = { 200, 200, 400, 200 };
    SDL_Surface *cursorSurface = IMG_Load("images/mouse9.png");
    SDL_Cursor *customCursor = SDL_CreateColorCursor(cursorSurface,0,0);
    SDL_SetCursor(customCursor);
    
    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT,2,2048);
    Mix_VolumeMusic(100);
    Mix_Music* music = Mix_LoadMUS("sounds/background.ogg");
    Mix_PlayMusic(music,-1);
    Mix_Chunk * win=Mix_LoadWAV("sounds/mouseclick.wav");
    Mix_VolumeChunk(win,100);
    Mix_Chunk * exit=Mix_LoadWAV("sounds/by.wav");
    Mix_VolumeChunk(win,100);
    
    SDL_SetRenderDrawColor(rendrer,255,0,0,255);
    window=SDL_CreateWindow("sdlproject",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_HEIGHT,SCREEN_WIDTH,SDL_WINDOW_FULLSCREEN);
    rendrer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    background=IMG_Load("images/background.png");
    center.h=SCREEN_HEIGHT;
    center.w=SCREEN_WIDTH;
    center.x = (SCREEN_WIDTH - center.w) / 2;  // Center horizontally
    center.y = (SCREEN_HEIGHT - center.h) / 2; // Center vertically
    SDL_KeyCode key;
    TTF_Init();
    
    TTF_Font *Font=TTF_OpenFont("fonts/ARIBLK.TTF",65);
    SDL_Color textColor= { 0,0,0},texth= { 0,114,255};
    SDL_Surface *startButtonSurface = TTF_RenderText_Solid(Font, "START", textColor);
    SDL_Surface *exitButtonSurface = TTF_RenderText_Solid(Font, "EXIT", textColor);
    SDL_Texture *startButtonTexture = SDL_CreateTextureFromSurface(rendrer, startButtonSurface);
    SDL_Texture *exitButtonTexture = SDL_CreateTextureFromSurface(rendrer, exitButtonSurface);
    SDL_Surface *inserButtonSurface = TTF_RenderText_Solid(Font, "Insert", textColor);
    SDL_Surface *affichButtonSurface = TTF_RenderText_Solid(Font, "Diplay", textColor);
    SDL_Surface *recherchButtonSurface = TTF_RenderText_Solid(Font, "Search", textColor);
    SDL_Surface *supprimButtonSurface = TTF_RenderText_Solid(Font, "Delete", textColor);
    SDL_Surface *quitButtonSurface = TTF_RenderText_Solid(Font, "Exit", textColor);
    SDL_Surface *idkButtonSurface = TTF_RenderText_Solid(Font, "Enter your number:", textColor);
    //SDL_Surface *inputButtonSurface = TTF_RenderText_Solid(Font, input, textColor);
    SDL_Texture *insereButtonTexture = SDL_CreateTextureFromSurface(rendrer, inserButtonSurface);
    SDL_Texture *affichButtonTexture = SDL_CreateTextureFromSurface(rendrer, affichButtonSurface);
    SDL_Texture *recherchButtonTexture = SDL_CreateTextureFromSurface(rendrer, recherchButtonSurface);
    SDL_Texture *supprimButtonTexture = SDL_CreateTextureFromSurface(rendrer, supprimButtonSurface);
    SDL_Texture *quitButtonTexture = SDL_CreateTextureFromSurface(rendrer, quitButtonSurface);
    SDL_Texture *idkButtonTexture = SDL_CreateTextureFromSurface(rendrer, idkButtonSurface);
    
    startButtonRect.x = 850;startButtonRect.y = 580;startButtonRect.w = startButtonSurface->w;startButtonRect.h = startButtonSurface->h;
    exitButtonRect.x = 880;exitButtonRect.y = 835;exitButtonRect.w = exitButtonSurface->w;exitButtonRect.h = exitButtonSurface->h;
    insereButtonRect.x =865;insereButtonRect.y=305;insereButtonRect.w= inserButtonSurface->w;insereButtonRect.h=inserButtonSurface->h;
    affichButtonRect.x =858;affichButtonRect.y=455;affichButtonRect.w=affichButtonSurface->w;affichButtonRect.h=affichButtonSurface->h;
    recherchButtonRect.x=842;recherchButtonRect.y=575;recherchButtonRect.w=recherchButtonSurface->w;recherchButtonRect.h=recherchButtonSurface->h;
    supprimButtonRect.x=847;supprimButtonRect.y=710;supprimButtonRect.w=supprimButtonSurface->w;supprimButtonRect.h=supprimButtonSurface->h;
    quitButtonRect.x=884;quitButtonRect.y=911;quitButtonRect.w=quitButtonSurface->w;quitButtonRect.h=quitButtonSurface->h;
    idkButtonRect.x=651;idkButtonRect.y=497;idkButtonRect.w=idkButtonSurface->w;idkButtonRect.h=idkButtonSurface->h;
    
    SDL_Event event;
    RUNNING=1;
    int n;
    gScreenSurface=SDL_GetWindowSurface(window);
    png_image=IMG_Load("images/start.png");
    png_image1=IMG_Load("images/idk.png");
    png_image2=IMG_Load("images/idk2.png");
    png_image3=IMG_Load("images/idk3.png");
    while (RUNNING!=0 )
    {
        
        SDL_BlitSurface(background, NULL,gScreenSurface, NULL);
        SDL_BlitSurface(startButtonSurface, NULL,gScreenSurface,&startButtonRect);
        SDL_BlitSurface(exitButtonSurface, NULL,gScreenSurface,&exitButtonRect);
        
        
        SDL_UpdateWindowSurface( window );

        while(SDL_PollEvent(&event)){
            switch(event.type){
            case SDL_QUIT:
                RUNNING = 0;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym){
                case SDLK_ESCAPE:
                    RUNNING=0;
                    break;
                case SDLK_F1:
                    RUNNING=0;
                    break;
                }
            
            case SDL_MOUSEBUTTONUP:
                
                if(event.button.x>772 && event.button.x<1151 && event.button.y>552 && event.button.y<683 ){
                    
                    Mix_PlayChannel(-1,win,0);
                    SDL_Delay(300) ;
                    handleMouseMotion=false;
                    while (RUNNING!=0)
                    {
                        
                        SDL_BlitSurface(png_image,NULL,gScreenSurface,&center);
                        SDL_BlitSurface(inserButtonSurface, NULL,gScreenSurface,&insereButtonRect);
                        SDL_BlitSurface(affichButtonSurface, NULL,gScreenSurface,&affichButtonRect);
                        SDL_BlitSurface(recherchButtonSurface, NULL,gScreenSurface,&recherchButtonRect);
                        SDL_BlitSurface(supprimButtonSurface, NULL,gScreenSurface,&supprimButtonRect);
                        SDL_BlitSurface(quitButtonSurface, NULL,gScreenSurface,&quitButtonRect);
                        SDL_UpdateWindowSurface( window );
                        k=false;
                        while(SDL_PollEvent(&event)){
                            switch(event.type){
                                case SDL_QUIT:RUNNING = 0;break;
                                case SDL_KEYDOWN: 
                                    switch (event.key.keysym.sym){case SDLK_ESCAPE:RUNNING=0;break;
                                        case SDLK_F1:RUNNING=0;break;}
                                
                                case SDL_MOUSEBUTTONUP:
                                    if (event.button.x>772 && event.button.x<1151 && event.button.y>300 && event.button.y<410)
                                    {
                                        
                                        Mix_PlayChannel(-1,win,0);
                                        SDL_Delay(300) ;
                                        strcpy(textBuffer,"");
                                        while (RUNNING!=0 && k==false)
                                        {
                                            
                                            SDL_Rect textRect;SDL_Surface* numsurface;SDL_Color textColor = {39, 56, 70};
                                            
                                            while (SDL_PollEvent(&event) )
                                            {
                                                x=881;y=809;
                                                switch (event.type )
                                                {
                                                case SDL_QUIT:
                                                    RUNNING=0;
                                                    break;
                                                    case SDL_TEXTINPUT:
                                                        if(strlen(textBuffer)<=5){
                                                        if (isInteger(event.text.text)){
                                                        strcat(textBuffer, event.text.text);
                                                        
                                                        
                                                        case SDL_KEYDOWN :
                                                            key = event.key.keysym.sym;
                                                            if(key == SDLK_KP_ENTER ) {k=true;n = atoi(textBuffer);root=insertNode(root,n);}
                                                            else if(key ==SDLK_ESCAPE){k=true;}
                                                            else if(key == SDLK_F1) RUNNING = 0;
                                                            textRect.x = x;
                                                            textRect.y = y;
                                                            
                                                            break;

                                                        break;    
                                                        }
                                                        }
                                                        case SDLK_BACKSPACE:
                                                            DEL(textBuffer,strlen(textBuffer)-1);
                                                }
                                            }
                                            idkButtonRect.x=651;idkButtonRect.y=497;
                                            SDL_BlitSurface(png_image1,NULL,gScreenSurface,NULL);
                                            SDL_BlitSurface(idkButtonSurface, NULL,gScreenSurface,&idkButtonRect);
                                            SDL_BlitSurface(TTF_RenderText_Solid(Font,textBuffer,textColor), NULL,gScreenSurface,&textRect);
                                            SDL_UpdateWindowSurface(window);
                                        }
                                    }
                                    if (event.button.x>772 && event.button.x<1151 && event.button.y>443 && event.button.y<554)
                                    {
                                        Mix_PlayChannel(-1,win,0);
                                        SDL_Delay(300) ;
                                        while (RUNNING!=0 && k==false)
                                        {
                                            while (SDL_PollEvent(&event) )
                                            {
                                                x=SCREEN_WIDTH/2;y=100;
                                                switch (event.type )
                                                {
                                                case SDL_QUIT:
                                                    RUNNING=0;
                                                    break;
                                                    
                                                case SDL_KEYDOWN :
                                                    key = event.key.keysym.sym;
                                                    if(key == SDLK_KP_ENTER) {
                                                        k=true; }
                                                    else if(key ==SDLK_ESCAPE ){k=true;}
                                                    else if(key == SDLK_F1) {RUNNING = 0;
                                                    }
                                                }
                                                
                                                break;
                                                }
                                            SDL_BlitSurface(png_image2,NULL,gScreenSurface,NULL);
                                            drawBinaryTree(rendrer,root,x,y,50,150,50,50,gScreenSurface);
                                            SDL_UpdateWindowSurface(window);
                                        }
                                        
                                    }
                                    if(event.button.x>772 && event.button.x<1151 && event.button.y>576 && event.button.y<684){
                                        if (s==false){Mix_PlayChannel(-1,win,0);
                                        SDL_Delay(300) ;}
                                        strcpy(textBuffer,"");
                                        while (RUNNING!=0 && k==false)
                                        {
                                            
                                            SDL_Rect textRect;SDL_Rect DE ;SDL_Surface* numsurface;SDL_Color textColor = {39, 56, 70};
                                            
                                            while (SDL_PollEvent(&event) )
                                            {
                                                
                                                x=800;y=600;
                                                switch (event.type )
                                                {
                                                case SDL_QUIT:
                                                    RUNNING=0;
                                                    break;
                                                    case SDL_TEXTINPUT:
                                                        if(strlen(textBuffer)<=5){
                                                        if (isInteger(event.text.text)){
                                                        strcat(textBuffer, event.text.text);
                                                        
                                                        case SDL_KEYDOWN :
                                                            key = event.key.keysym.sym;
                                                            if(key == SDLK_KP_ENTER ) {
                                                                
                                                                n = atoi(textBuffer);
                                                                foundNode = searchNode(root, n);
                                                                
                                                                if (foundNode != NULL) {
                                                                    l=0;
                                                                } else {
                                                                    l=1;
                                                                }
                                                                
                                                                
                                                            }
                                                            else if(key == SDLK_F2){s=true;}
                                                            else if(key ==SDLK_ESCAPE){k=true;}
                                                            else if(key == SDLK_F1) RUNNING = 0;
                                                            textRect.x = x;
                                                            textRect.y = y;
                                                            
                                                        }
                                                    break;
                                                }
                                                
                                                case SDLK_BACKSPACE:
                                                    DEL(textBuffer,strlen(textBuffer)-1);
                                                }
                                                
                                            }
                                            idkButtonRect.x=90;idkButtonRect.y=593;
                                            DE.x=1300;DE.y=399;
                                            
                                            SDL_BlitSurface(png_image3,NULL,gScreenSurface,NULL);
                                            SDL_BlitSurface(idkButtonSurface, NULL,gScreenSurface,&idkButtonRect);
                                            SDL_BlitSurface(TTF_RenderText_Solid(Font, textBuffer, textColor), NULL,gScreenSurface,&textRect);
                                            SDL_BlitSurface(TTF_RenderText_Solid(Font, "Not Found", textColor), NULL,gScreenSurface,&DE);
                                            DE.x=1353;DE.y=786;
                                            SDL_BlitSurface(TTF_RenderText_Solid(Font, "Found", textColor), NULL,gScreenSurface,&DE);
                                            if (l==1){
                                                SDL_Color textColo = {255, 0, 0};
                                                DE.x=1300;DE.y=399;
                                                SDL_BlitSurface(TTF_RenderText_Solid(Font, "Not Found", textColo), NULL,gScreenSurface,&DE);
                                            }
                                            else if (l==0){
                                                SDL_Color textColos = {0, 255, 0};
                                                DE.x=1353;DE.y=786;
                                                SDL_BlitSurface(TTF_RenderText_Solid(Font, "Found", textColos), NULL,gScreenSurface,&DE);
                                            }
                                            if (s==true &&l==0)
                                            {
                                                
                                                while (RUNNING!=0 && k==false)
                                                {
                                                    
                                                    while (SDL_PollEvent(&event) )
                                                    {
                                                        x=SCREEN_WIDTH/2;y=100;
                                                        switch (event.type )
                                                        {
                                                        case SDL_QUIT:
                                                            RUNNING=0;
                                                            break;
                                                            
                                                        case SDL_KEYDOWN :
                                                            key = event.key.keysym.sym;
                                                            if(key == SDLK_KP_ENTER) {
                                                                s=false; }
                                                            else if(key ==SDLK_ESCAPE ){s=false;}
                                                            else if(key == SDLK_F1) {RUNNING = 0;
                                                            }
                                                        }
                                                        
                                                        break;
                                                        }
                                                    SDL_BlitSurface(png_image2,NULL,gScreenSurface,NULL);
                                                    drawBinaryTreen(rendrer,root,x,y,n,50,150,50,50,gScreenSurface,s);
                                                    SDL_UpdateWindowSurface(window);
                                                    s==false;
                                                    break;
                                                }
                                                k=false;
                                            
                                            }
                                            
                                            SDL_UpdateWindowSurface(window);
                                            
                                        }
                                        
                                    }
                                    
                                    if (event.button.x>772 && event.button.x<1151 && event.button.y>710 && event.button.y<816)
                                    {
                                        Mix_PlayChannel(-1,win,0);
                                        SDL_Delay(300) ;
                                        strcpy(textBuffer,"");
                                        while (RUNNING!=0 && k==false)
                                        {
                                            SDL_Rect textRect,DE;SDL_Surface* numsurface;SDL_Color textColor = {39, 56, 70};
                                            
                                            while (SDL_PollEvent(&event) )
                                            {
                                                x=800;y=600;
                                                switch (event.type )
                                                {
                                                case SDL_QUIT:
                                                    RUNNING=0;
                                                    break;
                                                    case SDL_TEXTINPUT:
                                                        if(strlen(textBuffer)<=1){
                                                        if (isInteger(event.text.text)){
                                                        strcat(textBuffer, event.text.text);
                                                        
                                                case SDL_KEYDOWN :
                                                    key = event.key.keysym.sym;
                                                    if(key == SDLK_KP_ENTER ) {
                                                        n = atoi(textBuffer);
                                                        foundNode =searchNode(root,n);
                                                        if (foundNode!=NULL){l1=0;root =deleteNode(root,n);}
                                                        else if (foundNode ==NULL){l1=1;}
                                                    }
                                                    else if(key ==SDLK_ESCAPE){k=true;}
                                                    else if(key == SDLK_F1) RUNNING = 0;
                                                    textRect.x = x;
                                                    textRect.y = y;
                                                    }
                                                    break;
                                                }
                                                }
                                            }
                                            idkButtonRect.x=90;idkButtonRect.y=593;
                                            DE.x=1300;DE.y=399;
                                            SDL_BlitSurface(png_image3,NULL,gScreenSurface,NULL);
                                            SDL_BlitSurface(idkButtonSurface, NULL,gScreenSurface,&idkButtonRect);
                                            SDL_BlitSurface(TTF_RenderText_Solid(Font, textBuffer, textColor), NULL,gScreenSurface,&textRect);
                                            SDL_BlitSurface(TTF_RenderText_Solid(Font, "Doesn't Exist", textColor), NULL,gScreenSurface,&DE);
                                            DE.x=1353;DE.y=786;
                                            SDL_BlitSurface(TTF_RenderText_Solid(Font, "Done!!", textColor), NULL,gScreenSurface,&DE);
                                            if (l1==1){
                                                SDL_Color textColo = {255, 0, 0};
                                                DE.x=1300;DE.y=399;
                                                SDL_BlitSurface(TTF_RenderText_Solid(Font, "Doesn't Exist", textColo), NULL,gScreenSurface,&DE);
                                                
                                            }
                                            else if (l1==0){
                                                SDL_Color textColos = {0, 255, 0};
                                                DE.x=1353;DE.y=786;
                                                SDL_BlitSurface(TTF_RenderText_Solid(Font, "Done!!", textColos), NULL,gScreenSurface,&DE);
                                                
                                            }
                                            SDL_UpdateWindowSurface(window);
                                        }
                                    }
                            if(event.button.x>772 && event.button.x<1151 && event.button.y>889 && event.button.y<1150){RUNNING=0;Mix_PlayChannel(-1,exit,0);SDL_Delay(1500) ;}
                            break;
                        case SDL_MOUSEMOTION:
                            if (event.button.x>772 && event.button.x<1151 && event.button.y>300 && event.button.y<410)
                            {
                                inserButtonSurface = TTF_RenderText_Solid(Font, "Insert", texth);
                            }else {inserButtonSurface = TTF_RenderText_Solid(Font, "Insert", textColor);}
                            if (event.button.x>772 && event.button.x<1151 && event.button.y>443 && event.button.y<554)
                            {
                                affichButtonSurface = TTF_RenderText_Solid(Font, "Diplay", texth);
                            }else {affichButtonSurface = TTF_RenderText_Solid(Font, "Diplay", textColor);}
                            if (event.button.x>772 && event.button.x<1151 && event.button.y>576 && event.button.y<684)
                            {
                                recherchButtonSurface = TTF_RenderText_Solid(Font, "Search", texth);
                            }else {recherchButtonSurface = TTF_RenderText_Solid(Font, "Search", textColor);}
                            if (event.button.x>772 && event.button.x<1151 && event.button.y>710 && event.button.y<816)
                            {
                                supprimButtonSurface = TTF_RenderText_Solid(Font, "Delete", texth);
                            }else {supprimButtonSurface = TTF_RenderText_Solid(Font, "Delete", textColor);}
                            if (event.button.x>772 && event.button.x<1151 && event.button.y>889 && event.button.y<1150)
                            {
                                quitButtonSurface = TTF_RenderText_Solid(Font, "Exit", texth);
                            }else {quitButtonSurface = TTF_RenderText_Solid(Font, "Exit", textColor);}
                                
                            break;
                        }
                    }
                }
                }
                if (event.button.x>772 && event.button.x<1151 && event.button.y>810 && event.button.y<1050){RUNNING=0; Mix_PlayChannel(-1,exit,0);SDL_Delay(1500);}
                break;
                
            break;
            case SDL_MOUSEMOTION:
                if (event.button.x>772 && event.button.x<1151 && event.button.y>552 && event.button.y<683)
                {
                    startButtonSurface = TTF_RenderText_Solid(Font, "START", texth);
                }else {startButtonSurface = TTF_RenderText_Solid(Font, "START", textColor);}
                if (event.button.x>772 && event.button.x<1151 && event.button.y>810 && event.button.y<1050)
                {
                    exitButtonSurface = TTF_RenderText_Solid(Font, "EXIT", texth);
                }else {exitButtonSurface = TTF_RenderText_Solid(Font, "EXIT", textColor);}
                
            }
        }
    
    }
    SDL_FreeSurface(background);
    TTF_CloseFont(Font);
    SDL_DestroyRenderer(rendrer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
