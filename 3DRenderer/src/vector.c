#include <math.h>
#include "vector.h"
////////////////////////////////////////
/// Vector 2D functions Implementations
////////////////////////////////////////
float vec2_length(vec2_t v){
    return sqrt((v.x * v.x) + (v.y * v.y));
}

vec2_t vec2_add(vec2_t a, vec2_t b){
    vec2_t x = {
        .x = a.x + b.x,
        .y = a.y + b.y
    };
    return x;
}

vec2_t vec2_sub(vec2_t a, vec2_t b){
    vec2_t x = {
        .x = a.x - b.x,
        .y = a.y - b.y
    };
    return x;
}

vec2_t vec2_mult(vec2_t a, float f){
    vec2_t x = {
        .x = a.x * f,
        .y = a.y * f
    };
    return x;
}

vec2_t vec2_div(vec2_t a, float f){
    vec2_t x = {
        .x = a.x / f,
        .y = a.y / f
    };
    return x;
}
void vec2_normalize(vec2_t* v){
    float l = vec2_length(*v);
    v->x /= l;
    v->y /= l;
}

float vec2_dot(vec2_t a, vec2_t b){
    return (a.x * b.x) + (a.y * b.y);
}

////////////////////////////////////////
/// Vector 2D functions Implementations
////////////////////////////////////////
float vec3_length(vec3_t v){
    return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

vec3_t vec3_add(vec3_t a, vec3_t b){
    vec3_t x = {
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };
    return x;
}

vec3_t vec3_sub(vec3_t a, vec3_t b){
    vec3_t x = {
        .x = a.x - b.x,
        .y = a.y - b.y, 
        .z = a.z - b.z
    };
    return x;
}

vec3_t vec3_mult(vec3_t a, float f){
    vec3_t x = {
        .x = a.x * f,
        .y = a.y * f,
        .z = a.z * f
    };
    return x;
}

vec3_t vec3_div(vec3_t a, float f){
    vec3_t x = {
        .x = a.x / f,
        .y = a.y / f,
        .z = a.z / f
    };
    return x;
}

float vec3_dot(vec3_t a, vec3_t b){
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

vec3_t vec3_cross(vec3_t a, vec3_t b){
    vec3_t result = {
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    };

    return result;
}

void vec3_normalize(vec3_t* v){
    float l = vec3_length(*v);
    v->x /= l;
    v->y /= l;
    v->z /= l;
}

 vec3_t vec3_rotate_x(vec3_t v, float angle){
    vec3_t rotate_vector = {
        .x = v.x,
        .y = v.y * cos(angle) - v.z * sin(angle),
        .z = v.y * sin(angle) + v.z * cos(angle)
    };
    return rotate_vector;
 }

 vec3_t vec3_rotate_y(vec3_t v, float angle){
    vec3_t rotate_vector = {
        .x = v.x * cos(angle) - v.z * sin(angle),
        .y = v.y,
        .z = v.x * sin(angle) + v.z * cos(angle)
    };
    return rotate_vector;
 }
 
 vec3_t vec3_rotate_z(vec3_t v, float angle){
    vec3_t rotate_vector = {
        .x = v.x * cos(angle) - v.y * sin(angle),
        .y = v.x * sin(angle) + v.y * cos(angle),
        .z = v.z
    };
    return rotate_vector;
 }