#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <C:\SDL2\include\SDL.h>
#include "array.h"
#include "display.h"
#include "upng.h"
#include "vector.h"
#include "mesh.h"
#include "light.h"
#include <math.h>
#include "matrix.h"
#include "texture.h"
#include "triangle.h"
#include "camera.h"
#include "clipping.h"

bool isRunning = false; 
float delta_time;
mat4_t world_matrix;
mat4_t projection_matrix;
mat4_t view_matrix; 
int previous_frame_time = 0;


#define MAX_TRIANGLES_PER_MESH  10000
triangle_t  triangles_to_render[MAX_TRIANGLES_PER_MESH];

int num_triangles_to_render = 0;

uint32_t col_lerp(uint32_t a, uint32_t b, float t){
    return (uint32_t)(a + t * (b-a));
}



enum cull_method{
    CULL_NONE,
    CULL_BACKFACE
} cull_method;

enum render_method{
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE,
    RENDER_TEXTURED,
    RENDER_TEXTURED_WIRE
} render_method;


enum render_method render_mode;
enum cull_method cull_mode;

bool setup(void){
    // Set Initial Render Modes
    cull_mode = CULL_BACKFACE;
    render_mode = RENDER_WIRE;
     
   // Initialize Light Direction
    init_light(vec3_new(0,0,1));

    
    //Initialize projection matrix
    float aspect_x = (float)get_window_width()/(float)get_window_height();
    float aspect_y = (float)get_window_height()/(float)get_window_width();

    float fov_y = M_PI / 3.0;
    float fov_x = atan(tan(fov_y / 2) * aspect_x)*2;
    
    float znear = 0.1;
    float zfar = 100.0;
    projection_matrix = mat4_make_perspective(fov_y, aspect_y, znear, zfar);
   
    init_frustum_planes(fov_x, fov_y, znear, zfar);
    
    
    
    load_mesh("./assets/skull.obj","./assets/SamHead.png",vec3_new(0.5,0.5,0.5), vec3_new(-3,0,8), vec3_new(0,0,0));
    load_mesh("./assets/cube.obj","./assets/cube.png",vec3_new(1,1,1), vec3_new(3,0,8), vec3_new(0,0,0));

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Process the graphics pipeline in various stages
//  
// [ MODEL SPACE ] <- Original mesh vertices, all centered around the origin 0,0,0
//      
//      [ WORLD SPACE ] <- Multiply vertices by world matrix, essentially "places" meshes in the correct spot in the scene
//          
//          [ CAMERA SPACE ] <- Multiple vertices by the view matrix, essentially putting them in view relative to the camera
//      
//              [ CLIPPING ] <- Clip any vertices in our meshes that are outside the bounds of our frustum plane
//
//                  [ PROJECTION ] <- Multiple by the projection matrix, flatten our 3D world into a 2D image
//
//                      [ IMAGE SPACE ] <- Apply perspective divide on our meshes adding depth to the scene
//
//                          [ SCREEN SPACE ] <- Final image is now ready to be rendered to the screen
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void process_graphics_pipeline(mesh_t* mesh){
            vec3_t up = {0,1,0};
            vec3_t target = get_camera_lookat_target();
            

            view_matrix = mat4_look_at(get_camera_pos(),target,up);

            mat4_t scale_matrix =       mat4_make_scale(mesh->scale.x,mesh->scale.y,mesh->scale.z);
            mat4_t translation_matrix = mat4_make_translation(mesh->translation.x,mesh->translation.y,mesh->translation.z);
            
            mat4_t rotation_matrix_x = mat4_rotate_x(mesh->rotation.x);
            mat4_t rotation_matrix_y = mat4_rotate_y(mesh->rotation.y);
            mat4_t rotation_matrix_z = mat4_rotate_z(mesh->rotation.z);

            int num_faces = array_length(mesh->faces);
            for(int i = 0; i < num_faces; i ++){
                


                face_t current_face = mesh->faces[i];
                
                vec3_t face_vertices[3];

                face_vertices[0] = mesh->vertices[current_face.a ];
                face_vertices[1] = mesh->vertices[current_face.b ];
                face_vertices[2] = mesh->vertices[current_face.c ];


                vec4_t transformed_vertices[3];

                for(int j = 0; j < 3; j++){
                    vec4_t current_vertex = vec4_from_vec3(face_vertices[j]);

                    // Create a world matrix
                    world_matrix = mat4_identity();

                    // Multiply Scale -> Rotation -> Translation ORDER MATTERS >:(
                    world_matrix = matrix_mult_mat4(scale_matrix,      world_matrix);
                    world_matrix = matrix_mult_mat4(rotation_matrix_z, world_matrix);
                    world_matrix = matrix_mult_mat4(rotation_matrix_y, world_matrix);
                    world_matrix = matrix_mult_mat4(rotation_matrix_x, world_matrix);
                    
                    
                    world_matrix = matrix_mult_mat4(translation_matrix, world_matrix);

                    current_vertex = matrix_mult_vec4(world_matrix, current_vertex);

                    current_vertex = matrix_mult_vec4(view_matrix, current_vertex);
                    transformed_vertices[j] = current_vertex;
                    
                }
                vec3_t origin = {0,0,0};
                /////////////////
                //Cull back faces
                /////////////////
                vec3_t cameraRay = vec3_sub(origin,vec3_from_vec4(transformed_vertices[0]));
                vec3_t tri_normal = get_triangle_face_normal(transformed_vertices);
                vec3_normalize(&tri_normal);


                float orientation_from_camera = vec3_dot(tri_normal,cameraRay);
                if(cull_mode == CULL_BACKFACE){
                    if(orientation_from_camera < 0){
                        continue;
                    }
                }



                ////////////////
                //Clipping stage
                ////////////////

                // Create a polygon from the transformed triangle
                polygon_t polygon = create_polygon_from_triangle(
                    vec3_from_vec4(transformed_vertices[0]),
                    vec3_from_vec4(transformed_vertices[1]),
                    vec3_from_vec4(transformed_vertices[2]),
                    current_face.a_uv,
                    current_face.b_uv,
                    current_face.c_uv
                );

                // Clip polygon with the view frustum
                clip_polygon(&polygon);

                //printf("number of vertices after clipping: %d\n", polygon.num_vertices);
                
                // Break clipped polygon into triangles to be rendered
                triangle_t tris_after_clipping[MAX_NUM_POLY_TRIS];
                int num_triangles_after_clipping = 0;

                triangles_from_polygon(&polygon, tris_after_clipping, &num_triangles_after_clipping);

                // Loop through triangles after clipping

                for(int t = 0; t < num_triangles_after_clipping; t++){

                    triangle_t triangle_after_clipping = tris_after_clipping[t];

                    vec4_t projected_points[3];

                    for(int j = 0; j < 3; j++){
                        projected_points[j] = mat4_mul_vec4_project(projection_matrix,triangle_after_clipping.points[j]);
                        
                        
                        //Scale to middle of screen
                        projected_points[j].x *= get_window_width()/2.0;
                        projected_points[j].y *= get_window_height()/2.0;
                        

                        //Invert Y values to account for flipped y screen coordinate
                        projected_points[j].y *= -1;

                        //Translate to middle of screen
                        projected_points[j].x += get_window_width()/2.0;
                        projected_points[j].y += get_window_height()/2.0;
                    
                    }



                    ////////////////
                    //Lighting stage
                    ////////////////
                    
                    float orientation_from_light = -vec3_dot(tri_normal, get_light_direction());
                    uint32_t triangle_color = light_apply_intensity(0xFFFFFF,orientation_from_light);
                    orientation_from_light = orientation_from_light < 0.15 ? 0.15 : orientation_from_light;
                    orientation_from_light = orientation_from_light > 1.00 ? 1.00 : orientation_from_light;


                    
                    triangle_t triangle_to_render = {
                        .points = {
                            { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
                            { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
                            { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w },
                        },
                        .texcoords = {
                            {triangle_after_clipping.texcoords[0].u, triangle_after_clipping.texcoords[0].v},
                            {triangle_after_clipping.texcoords[1].u, triangle_after_clipping.texcoords[1].v},
                            {triangle_after_clipping.texcoords[2].u, triangle_after_clipping.texcoords[2].v}
                        },
                        .color = triangle_color,
                        .texture = mesh->texture
                        };
                    if(num_triangles_to_render < MAX_TRIANGLES_PER_MESH){
                        triangles_to_render[num_triangles_to_render] = triangle_to_render;
                        num_triangles_to_render++;
                    }
            }
        }
}




void update(void){
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks()-previous_frame_time);

    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
        SDL_Delay(time_to_wait);
    }
    //printf("Elapsed milliseconds : %d\n", SDL_GetTicks() - previous_frame_time);

    delta_time = (SDL_GetTicks() - previous_frame_time)/1000.0;

    previous_frame_time = SDL_GetTicks();

    //reset number of triangles to render each frame
    num_triangles_to_render = 0;
    for(int mesh_index = 0; mesh_index < get_num_meshes(); mesh_index++){
        process_graphics_pipeline(get_mesh(mesh_index));
    }        
}

void process_input(void){
    SDL_Event event;
    while (SDL_PollEvent(&event)) {;
        switch(event.type){
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_ESCAPE){
                    isRunning = false;
                }
                else if(event.key.keysym.sym == SDLK_w){
                    set_camera_vel(vec3_mult(get_camera_dir(),(float) 5.0 * delta_time));
                    set_camera_pos(vec3_add(get_camera_pos(),get_camera_vel()));
                }
                else if(event.key.keysym.sym == SDLK_s){
                    set_camera_vel(vec3_mult(get_camera_dir(), (float)5.0 * delta_time));
                    set_camera_pos(vec3_sub(get_camera_pos(),get_camera_vel()));
                }
                else if(event.key.keysym.sym == SDLK_UP)set_camera_pos(vec3_add(get_camera_pos(), vec3_new(0,12 * delta_time,0)));
                else if(event.key.keysym.sym == SDLK_DOWN)set_camera_pos(vec3_sub(get_camera_pos(), vec3_new(0,12 * delta_time,0)));
                else if(event.key.keysym.sym == SDLK_a){
                    rotate_camera_yaw(1.0 * delta_time);
                    
                }
                else if(event.key.keysym.sym == SDLK_d){
                    rotate_camera_yaw(-1.0 * delta_time);
                    
                }
                else if(event.key.keysym.sym == SDLK_j){
                    rotate_camera_pitch( -3.0 * delta_time);
                }
                else if(event.key.keysym.sym == SDLK_n){
                    rotate_camera_pitch( 3.0 * delta_time);
                    
                }
                else if(event.key.keysym.sym == SDLK_1)render_mode = RENDER_WIRE_VERTEX;
                else if(event.key.keysym.sym == SDLK_2)render_mode = RENDER_WIRE;
                else if(event.key.keysym.sym == SDLK_3)render_mode = RENDER_FILL_TRIANGLE;
                else if(event.key.keysym.sym == SDLK_4)render_mode = RENDER_FILL_TRIANGLE_WIRE;
                else if(event.key.keysym.sym == SDLK_5)render_mode = RENDER_TEXTURED;
                else if(event.key.keysym.sym == SDLK_6)render_mode = RENDER_TEXTURED_WIRE;
                else if(event.key.keysym.sym == SDLK_c) cull_mode = CULL_BACKFACE;
                else if(event.key.keysym.sym == SDLK_x) cull_mode = CULL_NONE;
                break;        
        }
    }
}

void render(void){
    clear_color_buffer_gradient(0x000000,0x000000);
    clear_z_buffer();
    draw_grid(10,40);
    
    
    for(int i = 0; i < num_triangles_to_render; i ++){
        triangle_t tri = triangles_to_render[i];
        //Draw based on the render mode
        switch(render_mode){
            case RENDER_WIRE_VERTEX:
                draw_triangle(
                tri.points[0].x, tri.points[0].y, // vertex A
                tri.points[1].x, tri.points[1].y, // vertex B
                tri.points[2].x, tri.points[2].y, // vertex C
                0xFFFFFF
                );
                drawRect(tri.points[0].x, tri.points[0].y, 5, 5, 0xFF0000);
                drawRect(tri.points[1].x, tri.points[1].y, 5, 5, 0xFF0000);
                drawRect(tri.points[2].x, tri.points[2].y, 5, 5, 0xFF0000);
            break;
            case RENDER_WIRE:
                  draw_triangle(
                    tri.points[0].x, tri.points[0].y, // vertex A
                    tri.points[1].x, tri.points[1].y, // vertex B
                    tri.points[2].x, tri.points[2].y, // vertex C
                    0xFFFFFF
                    );
            break;
            case RENDER_FILL_TRIANGLE:
                  draw_filled_triangle(
                tri.points[0].x, tri.points[0].y, tri.points[0].z ,tri.points[0].w,  // vertex A
                tri.points[1].x, tri.points[1].y, tri.points[1].z ,tri.points[1].w,  // vertex B
                tri.points[2].x, tri.points[2].y, tri.points[2].z ,tri.points[2].w,  // vertex C
                tri.color
                );
            break;
            case RENDER_FILL_TRIANGLE_WIRE:
                 draw_filled_triangle(
                tri.points[0].x, tri.points[0].y, tri.points[0].z ,tri.points[0].w,  // vertex A
                tri.points[1].x, tri.points[1].y, tri.points[1].z ,tri.points[1].w,  // vertex B
                tri.points[2].x, tri.points[2].y, tri.points[2].z ,tri.points[2].w,  // vertex C
            tri.color
            ); 
            draw_triangle(
            tri.points[0].x, tri.points[0].y, // vertex A
            tri.points[1].x, tri.points[1].y, // vertex B
            tri.points[2].x, tri.points[2].y, // vertex C
            0x000000
            );
            break;
            case RENDER_TEXTURED:
            draw_textured_triangle(
                tri.points[0].x, tri.points[0].y, tri.points[0].z ,tri.points[0].w, tri.texcoords[0].u, tri.texcoords[0].v, // vertex A
                tri.points[1].x, tri.points[1].y, tri.points[1].z ,tri.points[1].w, tri.texcoords[1].u, tri.texcoords[1].v, // vertex B
                tri.points[2].x, tri.points[2].y, tri.points[2].z ,tri.points[2].w, tri.texcoords[2].u, tri.texcoords[2].v,  // vertex C
                tri.texture
            );
            break;
            case RENDER_TEXTURED_WIRE:
             draw_textured_triangle(
                tri.points[0].x, tri.points[0].y, tri.points[0].z ,tri.points[0].w, tri.texcoords[0].u, tri.texcoords[0].v, // vertex A
                tri.points[1].x, tri.points[1].y, tri.points[1].z ,tri.points[1].w, tri.texcoords[1].u, tri.texcoords[1].v, // vertex B
                tri.points[2].x, tri.points[2].y, tri.points[2].z ,tri.points[2].w, tri.texcoords[2].u, tri.texcoords[2].v,  // vertex C
                tri.texture
            );
             draw_triangle(
            tri.points[0].x, tri.points[0].y, // vertex A
            tri.points[1].x, tri.points[1].y, // vertex B
            tri.points[2].x, tri.points[2].y, // vertex C
            0X000000
            );
            break;
        }
     
    }
    render_color_buffer();
   
}



int main(int argc, char* args[]){
    isRunning = initialize_window();

    setup();

    while (isRunning) {
        process_input();
        update();
        render();
    }
    free_meshes();
    destroy_window();
   
    return 0;
}