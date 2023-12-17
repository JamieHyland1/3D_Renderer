#ifndef CAMERA_H
#define CAMERA_H
#include "vector.h"
typedef struct{
    vec3_t position;
    vec3_t direction;
    vec3_t forward_velocity;
    float yaw_angle;
    float pitch_angle;
    float roll_angle;

} camera_t;


vec3_t get_camera_lookat_target(void);

vec3_t get_camera_pos(void);
vec3_t get_camera_dir(void);
vec3_t get_camera_vel(void);

void set_camera_pos(vec3_t pos);
void set_camera_dir(vec3_t dir);
void set_camera_vel(vec3_t vel);

float  get_camera_yaw(void);
float  get_camera_pitch(void);
float  get_camera_roll(void);

void  rotate_camera_yaw(float angle);
void  rotate_camera_pitch(float angle);
void  rotate_camera_roll(float angle);

#endif