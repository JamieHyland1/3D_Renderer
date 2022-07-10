#ifndef display_h
#define display_h
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <C:\SDL2\include\SDL.h>

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* color_buffer_texture;

extern uint32_t* color_buffer;

extern int window_width;
extern int window_height;

bool initialize_window(void);
void destroy_window(void);
void clear_color_buffer(uint32_t color);
void render_color_buffer(void);
void drawRect(int x, int y, int w, int h, uint32_t color);
void draw_grid(void);

#endif