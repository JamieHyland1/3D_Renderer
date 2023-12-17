#ifndef light_h
#define light_h
#include "vector.h"
typedef struct{
    vec3_t direction;
    float intensity;

}light_t;

void init_light(vec3_t direction);
vec3_t get_light_direction(void);
uint32_t light_apply_intensity(uint32_t color, float percentage_factor);

#endif