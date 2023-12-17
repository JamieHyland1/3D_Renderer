#ifndef display_h
#define display_h
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <C:\SDL2\include\SDL.h>
#include <math.h>

#define FPS 60
#define FRAME_TARGET_TIME  (1000 / FPS)

int  get_window_width(void);
int  get_window_height(void);

bool initialize_window(void);

void destroy_window(void);
void clear_color_buffer(uint32_t color);
void clear_color_buffer_gradient(uint32_t col1, uint32_t col2);
void clear_z_buffer();
void render_color_buffer(void);
void draw_pixel(int x, int y, uint32_t color);
void drawRect(int x, int y, int w, int h, uint32_t color);
void draw_grid(int xPlot, int rowSize);
void draw_boxed_grid(void);
void draw_line(int x1, int y1, int x2, int y2, uint32_t color);
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);

float get_z_buffer_at(int x, int y);
void update_zbuffer(int x, int y, float value);
#endif