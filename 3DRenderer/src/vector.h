#ifndef vector_h
#define vector_h
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <C:\SDL2\include\SDL.h>

typedef struct{
    float x;
    float y;
}vec2_t;

typedef struct{
    float x;
    float y;
    float z;
}vec3_t;

vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);


#endif