#ifndef matrix_h
#define matrix_h

typedef struct{
    float m[4][4];
}mat4_t;

mat4_t mat4_identity();
mat4_t mat4_make_scale(float sx, float sy, float sz);

#endif