#ifndef texture_h
#define texture_h
#include "upng.h"
#include <stdint.h>

typedef struct{
    float u;
    float v;
}tex2_t;

extern int texture_width;
extern int texture_height;

tex2_t new_tex2(float u, float v);
extern uint32_t* mesh_texture;
extern upng_t* png_texture;
void load_png_texture_data(char* filename);


#endif
