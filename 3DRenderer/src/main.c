#include <stdio.h>
#include <stdbool.h>
#include <C:\SDL2\include\SDL.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
bool isRunning = false; 

bool initialize_window(void){
    
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0){
        fprintf(stderr, "Error initializing SDL window");
        return false;
    }
    //TODO create SDL window
    window = SDL_CreateWindow(
        "The window into jamie's madness",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        400,
        400,
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



    return true;
}

void setup(void){

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

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
}
int main(int argc, char* args[]){
    // create an sdl window
    setup();
    isRunning = initialize_window();
   
    while(isRunning){
        process_input();
        update();
        render();
    }
    return 0;
}