#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <C:\SDL2\include\SDL.h>
#include "display.h"

bool isRunning = false; 
bool setup(void){
    printf("Setting up Renderer");
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );
    
    if(!color_buffer){
        fprintf(stderr, "couldnt allocate memory for color buffer");
        return false;
    }
    
    return true;
}

void update(void){

}

void process_input(void){
    SDL_Event event;
    SDL_PollEvent(&event);
    switch(event.type){
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE){
                isRunning = false;
            }
            break;
    }
}

void render(void){

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    
    SDL_RenderClear(renderer);
    
    draw_grid();

    drawRect(window_width/2,window_height/2,150,50,0xFFFF0000);
    
    render_color_buffer();
    
    clear_color_buffer(0x00000000);
   
    SDL_RenderPresent(renderer);
}

int main(int argc, char* args[]){
    isRunning = initialize_window();

    setup();

    while (isRunning) {
        process_input();
        update();
        render();
    }

    destroy_window();

    return 0;
}



// int main(int argc, char* args[]){