#include "matrix.h"
#include <math.h>
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

mat4_t mat4_make_translation(float tx, float ty, float tz){
    // swap the values in the identity matrix with our scale values 
    mat4_t translation_m = mat4_identity();
    translation_m.m[0][3] = tx;
    translation_m.m[1][3] = ty;
    translation_m.m[2][3] = tz;

    return translation_m;
}

mat4_t matrix_mult_mat4(mat4_t a, mat4_t b){
    mat4_t result;
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j ++){
            result.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
        }
    }

    return result;
}

vec4_t matrix_mult_vec4(mat4_t m, vec4_t v){
    vec4_t result;
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

    return result;
}
// Rotate around Z axis
// | 1    0       0     0 |   | x |
// | 0  cos(a) -sin(a)  0 |   | y |
// | 0  sin(a)  cos(a)  0 | * | z |
// | 0    0       0     1 |   | 1 |

mat4_t mat4_rotate_z(float angle){
    mat4_t m = mat4_identity();

    m.m[0][0] = cos(angle);
    m.m[0][1] = -sin(angle);
    m.m[1][0] = sin(angle);
    m.m[1][1] = cos(angle);

    return m;

}


// Rotation around X axis
// | 1    0       0     0 |   | x |
// | 0  cos(a) -sin(a)  0 |   | y | 
// | 0  sin(a)  cos(a)  0 | * | z |
// | 0    0       0     1 |   | 1 |

mat4_t mat4_rotate_x(float angle){
    mat4_t m = mat4_identity();

    m.m[1][1] =  cos(angle);
    m.m[1][2] = -sin(angle);
    m.m[2][1] =  sin(angle);
    m.m[2][2] =  cos(angle);

    return m;

}

// Rotation around Y axis
// |  cos(a)  0  sin(a)  0 |   | x |
// |    0     1    0     0 |   | y |
// | -sin(a)  0  cos(a)  0 | * | z |
// |    0     0    0     1 |   | 1 |
 
mat4_t mat4_rotate_y(float angle){
    mat4_t m = mat4_identity();

    m.m[0][0] =  cos(angle);
    m.m[0][2] =  sin(angle);
    m.m[2][0] = -sin(angle);
    m.m[2][2] =  cos(angle);

    return m;

}

