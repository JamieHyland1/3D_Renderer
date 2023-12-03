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


bool isRunning = false; 
mat4_t projection_matrix;
int previous_frame_time = 0;

vec3_t camera_pos = {0,0,0};

triangle_t*  triangles_to_render = NULL;

uint32_t col_lerp(uint32_t a, uint32_t b, float t){
    return (uint32_t)(a + t * (b-a));
}

light_t main_light = {.direction = {0,0,1}};

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

uint32_t t;
enum render_method render_mode;
enum cull_method cull_mode;

bool setup(void){
    cull_mode = CULL_BACKFACE;
    render_mode = RENDER_WIRE;
     
    printf("Setting up Renderer\n");
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );
    

    //Initialize projection matrix
    float fov = M_PI / 3.0;
    float aspect = (float)window_height/(float)window_width;
    float znear = 0.1;
    float zfar = 100.0;
    projection_matrix = mat4_make_perspective(fov, aspect, znear, zfar);

    //load_cube_mesh_data();
    load_obj_file_data("./assets/skull.obj");

    if(!color_buffer){
        fprintf(stderr, "couldnt allocate memory for color buffer");
        return false;
        
    }

load_png_texture_data("./assets/SamHead.png");


    return true;
}



void update(void){
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks()-previous_frame_time);

    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    triangles_to_render = NULL;

   // mesh.rotation.x += 0.005;
    mesh.rotation.y -= 0.005;
    mesh.translation.z = 5;
    

    //mesh.translation.z = -camera_pos.z;
   

    mat4_t scale_matrix =       mat4_make_scale(mesh.scale.x,mesh.scale.y,mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x,mesh.translation.y,mesh.translation.z);
    
    mat4_t rotation_matrix_x = mat4_rotate_x(mesh.rotation.x);
    mat4_t rotation_matrix_y = mat4_rotate_y(mesh.rotation.y);
    mat4_t rotation_matrix_z = mat4_rotate_z(mesh.rotation.z);

    int num_faces = array_length(mesh.faces);
    for(int i = 0; i < num_faces; i ++){
        
        face_t current_face = mesh.faces[i];
        
        vec3_t face_vertices[3];

        face_vertices[0] = mesh.vertices[current_face.a ];
        face_vertices[1] = mesh.vertices[current_face.b ];
        face_vertices[2] = mesh.vertices[current_face.c ];

        
       

        vec4_t transformed_vertices[3];

        for(int j = 0; j < 3; j++){
            vec4_t current_vertex = vec4_from_vec3(face_vertices[j]);

            // Create a world matrix
            mat4_t world_matrix = mat4_identity();

            // Multiply Scale -> Rotation -> Translation ORDER MATTERS >:(
            world_matrix = matrix_mult_mat4(scale_matrix,world_matrix);
            
            world_matrix = matrix_mult_mat4(rotation_matrix_x,world_matrix);
            world_matrix = matrix_mult_mat4(rotation_matrix_y,world_matrix);
            world_matrix = matrix_mult_mat4(rotation_matrix_z,world_matrix);

            world_matrix = matrix_mult_mat4(translation_matrix,world_matrix);
           
            transformed_vertices[j] = matrix_mult_vec4(world_matrix,current_vertex);
            
        }


        //Cull back faces
        vec3_t cameraRay = vec3_sub(camera_pos,vec3_from_vec4(transformed_vertices[0]));

        vec3_t v1 = vec3_sub(vec3_from_vec4(transformed_vertices[1]),vec3_from_vec4(transformed_vertices[0]));
        vec3_t v2 = vec3_sub(vec3_from_vec4(transformed_vertices[2]),vec3_from_vec4(transformed_vertices[0]));

        vec3_t tri_normal = vec3_cross(v1,v2);


        vec3_normalize(&tri_normal);
        vec3_normalize(&main_light.direction);


       
        float orientation_from_camera = vec3_dot(tri_normal,cameraRay);
      // printf("orientation from camera: %f", orientation_from_camera);
        if(cull_mode == CULL_BACKFACE){
            if(orientation_from_camera < 0){
                continue;
            }
        }



         float orientation_from_light = -vec3_dot(tri_normal, main_light.direction);
        uint32_t triangle_color = light_apply_intensity(0xFFFFFF,orientation_from_light);
        orientation_from_light = orientation_from_light < 0.15 ? 0.15 : orientation_from_light;
        orientation_from_light = orientation_from_light > 1.00 ? 1.00 : orientation_from_light;
        


        

        vec4_t projected_points[3];

        for(int j = 0; j < 3; j++){
            projected_points[j] = mat4_mul_vec4_project(projection_matrix,transformed_vertices[j]);
            
            
             //Scale to middle of screen
            projected_points[j].x *= window_width/2.0;
            projected_points[j].y *= window_height/2.0;
            

            //Invert Y values to account for flipped y screen coordinate
            projected_points[j].y *= -1;

            //Translate to middle of screen
            projected_points[j].x += window_width/2.0;
            projected_points[j].y += window_height/2.0;
           
        }
        
        triangle_t projected_triangle = {
            .points = {
                { projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
                { projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
                { projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w },
            },
            .avg_depth = ((transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0),
            .texcoords = {
                {current_face.a_uv.u, current_face.a_uv.v},
                {current_face.b_uv.u, current_face.b_uv.v},
                {current_face.c_uv.u, current_face.c_uv.v}
            },
            .color = triangle_color
            };


        array_push(triangles_to_render,projected_triangle);

        // Sort triangle in order of depth back to front
        // TODO : add better sorting algorithm 
        int num_triangles = array_length(triangles_to_render);
        for(int i = 0; i < num_triangles; i ++){
            for (int j = i+1; j < num_triangles; j ++){
                if(triangles_to_render[j].avg_depth > triangles_to_render[i].avg_depth){
                    triangle_t temp = triangles_to_render[i];
                    triangles_to_render[i] = triangles_to_render[j];
                    triangles_to_render[j] = temp;
                }
            }
        }
    }
}


void process_input(void){
    SDL_Event event;
    SDL_PollEvent(&event);
    switch(event.type){
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE){
                isRunning = false;
            }
            // else if(event.key.keysym.sym == SDLK_UP) camera_pos.z += 2.0;
            // else if(event.key.keysym.sym == SDLK_DOWN) camera_pos.z -= 2.0;
            else if(event.key.keysym.sym == SDLK_1)render_mode = RENDER_WIRE_VERTEX;
            else if(event.key.keysym.sym == SDLK_2)render_mode = RENDER_WIRE;
            else if(event.key.keysym.sym == SDLK_3)render_mode = RENDER_FILL_TRIANGLE;
            else if(event.key.keysym.sym == SDLK_4)render_mode = RENDER_FILL_TRIANGLE_WIRE;
            else if(event.key.keysym.sym == SDLK_5)render_mode = RENDER_TEXTURED;
            else if(event.key.keysym.sym == SDLK_6)render_mode = RENDER_TEXTURED_WIRE;
            else if(event.key.keysym.sym == SDLK_c) cull_mode = CULL_BACKFACE;
            else if(event.key.keysym.sym == SDLK_d) cull_mode = CULL_NONE;
            break;        
    }
}

void render(void){
   
    render_color_buffer();
    uint32_t col = 0x000000;

    clear_color_buffer_gradient(col,col);
     draw_grid(10,40);
    int num_triangles = array_length(triangles_to_render);
    
   
    for(int i = 0; i < num_triangles; i ++){
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
                drawRect(tri.points[0].x+camera_pos.x, tri.points[0].y+camera_pos.y, 5, 5, 0xFF0000);
                drawRect(tri.points[1].x+camera_pos.x, tri.points[1].y+camera_pos.y, 5, 5, 0xFF0000);
                drawRect(tri.points[2].x+camera_pos.x, tri.points[2].y+camera_pos.y, 5, 5, 0xFF0000);
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
                    tri.points[0].x, tri.points[0].y, // vertex A
                    tri.points[1].x, tri.points[1].y, // vertex B
                    tri.points[2].x, tri.points[2].y, // vertex C
                0xFFFFFF
                );
            break;
            case RENDER_FILL_TRIANGLE_WIRE:
                 draw_filled_triangle(
            tri.points[0].x, tri.points[0].y, // vertex A
            tri.points[1].x, tri.points[1].y, // vertex B
            tri.points[2].x, tri.points[2].y, // vertex C
            0xFFFFFF
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
                mesh_texture
            );
            break;
            case RENDER_TEXTURED_WIRE:
             draw_textured_triangle(
                tri.points[0].x, tri.points[0].y, tri.points[0].z ,tri.points[0].w, tri.texcoords[0].u, tri.texcoords[0].v, // vertex A
                tri.points[1].x, tri.points[1].y, tri.points[1].z ,tri.points[1].w, tri.texcoords[1].u, tri.texcoords[1].v, // vertex B
                tri.points[2].x, tri.points[2].y, tri.points[2].z ,tri.points[2].w, tri.texcoords[2].u, tri.texcoords[2].v,  // vertex C
                mesh_texture
            );
             draw_triangle(
            tri.points[0].x, tri.points[0].y, // vertex A
            tri.points[1].x, tri.points[1].y, // vertex B
            tri.points[2].x, tri.points[2].y, // vertex C
            0xFFFFFF
            );
            break;
        }
     
    }
    array_free(triangles_to_render);
    SDL_RenderPresent(renderer);
}


void free_resources(void){
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.face_normals);
    array_free(mesh.vertices);
    upng_free(png_texture);

}

int main(int argc, char* args[]){
    isRunning = initialize_window();

    setup();

    while (isRunning) {
        process_input();
        update();
        render();
    }
    
    destroy_window();
    free_resources();
    return 0;
}