#include "matrix.h"

mat4_t mat4_identity(){
    mat4_t I = {{
        {1,0,0,0},
        {0,1,0,0},
        {0,0,1,0},
        {0,0,0,1}
    }};

    return I;
}

mat4_t mat4_make_scale(float sx, float sy, float sz){
    // swap the values in the identity matrix with our scale values 
    mat4_t scale_m = mat4_identity();
    scale_m.m[0][0] = sx;
    scale_m.m[1][1] = sy;
    scale_m.m[2][2] = sz;

    return scale_m;
}