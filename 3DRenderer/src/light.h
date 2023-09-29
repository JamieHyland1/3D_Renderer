#ifndef light_h
#define light_h
#include "vector.h"
typedef struct{
    vec3_t direction;
    float intensity;

}light_t;


uint32_t light_apply_intensity(uint32_t color, float percentage_factor);

#endif