#include "camera.h"
#include "matrix.h"
static camera_t camera = {
    .position = {0,0,0},
    .direction = {0,0,1},
    .forward_velocity = {0,0,0},
    .yaw_angle = 0.0,
    .pitch_angle = 0.0,
    .roll_angle = 0.0
};

vec3_t get_camera_pos(void){
    return camera.position;
}

vec3_t get_camera_dir(void){
    return camera.direction;
}
vec3_t get_camera_vel(void){
    return camera.forward_velocity;
}

void set_camera_pos(vec3_t pos){
    camera.position = pos;
}
void set_camera_dir(vec3_t dir){
    camera.direction = dir;
}
void set_camera_vel(vec3_t vel){
    camera.forward_velocity = vel;
}

float  get_camera_yaw(void){
    return camera.yaw_angle;
}
float  get_camera_pitch(void){
    return camera.pitch_angle;
}
float  get_camera_roll(void){
    return camera.roll_angle;
}

void  rotate_camera_yaw(float angle){
    camera.yaw_angle += angle;
}
void  rotate_camera_pitch(float angle){
    camera.pitch_angle += angle;
}
void  rotate_camera_roll(float angle){
    camera.roll_angle += angle;
}

vec3_t get_camera_lookat_target(){
    vec3_t target = {0,0,1};
   
    
    mat4_t camera_yaw_rotation = mat4_rotate_y(camera.yaw_angle);
    mat4_t camera_pitch_rotation = mat4_rotate_x(camera.pitch_angle);

    mat4_t camera_rotation = mat4_identity();

    camera_rotation = matrix_mult_mat4(camera_yaw_rotation,camera_rotation);
    camera_rotation = matrix_mult_mat4(camera_pitch_rotation,camera_rotation);

    vec4_t camera_direction = matrix_mult_vec4(camera_rotation, vec4_from_vec3(target));

    camera.direction = vec3_from_vec4(camera_direction);


    target = vec3_add(camera.position,camera.direction);

    return target;

}