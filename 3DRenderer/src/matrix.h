#ifndef matrix_h
#define matrix_h
#include "vector.h"
typedef struct{
    float m[4][4];
}mat4_t;

mat4_t mat4_identity();
mat4_t mat4_make_scale(float sx, float sy, float sz);
mat4_t mat4_make_translation(float tx, float ty, float tz);
mat4_t matrix_mult_mat4(mat4_t a, mat4_t b);
vec4_t matrix_mult_vec4(mat4_t m, vec4_t v);

///////////
//Rotations
///////////

mat4_t mat4_rotate_z(float angle);
mat4_t mat4_rotate_x(float angle);
mat4_t mat4_rotate_y(float angle);
#endif