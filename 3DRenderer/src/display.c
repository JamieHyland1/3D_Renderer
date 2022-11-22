#include "display.h"
#include <math.h>
// This file will contain the functions necessary to display our renderer to the screen
// It will contain various SDL functions and functions related to drawing various primitive shapes

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;


//declare an array to uint32 elements
uint32_t* color_buffer = NULL;

int window_width = 640;
int window_height = 480;



bool initialize_window(void){
    
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        fprintf(stderr, "Error initializing SDL window");
        return false;
    }

    SDL_DisplayMode displayMode;

    SDL_GetCurrentDisplayMode(0, &displayMode);

    window_width = displayMode.w;
    window_height = displayMode.h;

    //TODO create SDL window
    window = SDL_CreateWindow(
        "The window into jamie's madness",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_FOREIGN
    );

    if(!window){
        fprintf(stderr, "Error creating SDL window");
        return false;
    }

    //TODO create SDL Renderer
    renderer = SDL_CreateRenderer(window,-1, 0);

    if(!renderer){
        fprintf(stderr, "Error creating SDL renderer");
        return false;
    }

    SDL_SetWindowFullscreen(window,SDL_WINDOW_BORDERLESS);

    return true;
}


void clear_color_buffer(uint32_t color){
    for(int y = 0; y < window_height; y++){
        for(int x = 0; x < window_width; x++){
            int index = (window_width * y) + x;

            color_buffer[index] = color;
        }
    }
}
int lerp(float a, float b, float t){
    return (int) a + t * (b - a);
}
void clear_color_buffer_gradient(uint32_t col1, uint32_t col2){
    float t = 0;
 
    int r1 = col1 >> 16 & 0xFF;
    int g1 = col1 >> 8 & 0xFF;
    int b1 = col1 & 0xFF;


    int r2 = col2 >> 16 & 0xFF;
    int g2 = col2 >> 8 & 0xFF;
    int b2 = col2 & 0xFF;

    int nr = lerp((float)r2,(float)r1,0);
    int ng = lerp((float)g2,(float)g1,0);
    int nb = lerp((float)b2,(float)b1,0);
     for(int y = 0; y < window_height; y++){
        for(int x = 0; x < window_width; x++){
            int index = (window_width * y) + x;

            uint32_t new_col = (255 << 24) + (nr << 16) + (ng << 8) + nb;
            color_buffer[index] = new_col;
        }
        
        t = (float)y/window_height; 
        nr = lerp((float)r2,(float)r1,t);
        ng = lerp((float)g2,(float)g1,t);
        nb = lerp((float)b2,(float)b1,t);
    }
}
void drawRect(int x, int y, int w, int h, uint32_t color){
    for(int j = y; j <= y+h; j++){
        for(int i = x; i <= x+w; i++){
            if((i <= window_width  && i >= 0) && (j <= window_height && j >= 0)){ 
                drawPixel(i,j,color);
            }
        }
    }
}

void draw_grid(void){
    for(int y = 0; y < window_height; y++){
        for(int x = 0; x < window_width; x++){
            int index = (window_width * y) + x;
            if(x%40 == 0  || y%40 == 0)color_buffer[index] = 0xFFFFFFFF;
        }
    }
}

void drawPixel(int x, int y, uint32_t color){
    if((x < window_width  && x >= 0) && (y < window_height && y >= 0)){    
        int index = (y*window_width) + x;
        color_buffer[index] = color;
    }
}
void render_color_buffer(void){
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(window_width * sizeof (uint32_t))
    );

    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void draw_line(int x1, int y1, int x2, int y2, uint32_t color){
    int deltaX = x2-x1;
    int deltaY = y2-y1;

    int side_length = abs(deltaX) >= abs(deltaY) ? abs(deltaX) : abs(deltaY);

    float x_inc = deltaX / (float)side_length;
    float y_inc = deltaY / (float)side_length;

    float currentX = x1;
    float currentY = y1;

    for(int i = 0; i < side_length; i ++){
        drawPixel(round(currentX),round(currentY), color);
        currentX += x_inc;
        currentY += y_inc;
    }
}

void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color){
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x3, y3, color);
    draw_line(x3, y3, x1, y1, color);
}

void destroy_window(void){
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
