#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <C:\SDL2\include\SDL.h>
#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include <math.h>

bool isRunning = false; 
int fov_factor = 640;
int previous_frame_time = 0;

vec3_t camera_pos = {0,3,-5};

triangle_t*  triangles_to_render = NULL;

uint32_t col_lerp(uint32_t a, uint32_t b, float t){
    return (uint32_t)(a + t * (b-a));
}

bool setup(void){
    printf("Setting up Renderer\n");
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    // printf("Frame target time:\n");
    // printf("%d\n", FRAME_TARGET_TIME);
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    
    load_obj_file_data("./assets/skull.obj");

    if(!color_buffer){
        fprintf(stderr, "couldnt allocate memory for color buffer");
        return false;
    }

    return true;
}

vec2_t project(vec3_t v){
    vec2_t projected = {
        .x = (v.x * fov_factor)/v.z,
        .y = (v.y * fov_factor)/v.z
    };
    return projected;
}

void update(void){
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks()-previous_frame_time);

    if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
        SDL_Delay(time_to_wait);
    }

    previous_frame_time = SDL_GetTicks();

    triangles_to_render = NULL;

    //mesh.rotation.x = M_PI/4;
    mesh.rotation.y += 0.01;
    //mesh.rotation.z += 0.01;
    int num_faces = array_length(mesh.faces);
    for(int i = 0; i < num_faces; i ++){
        
        face_t current_face = mesh.faces[i];
        
        vec3_t face_vertices[3];

        face_vertices[0] = mesh.vertices[current_face.a - 1];
        face_vertices[1] = mesh.vertices[current_face.b - 1];
        face_vertices[2] = mesh.vertices[current_face.c - 1];

       

        vec3_t transformed_vertices[3];

        for(int j = 0; j < 3; j++){
            vec3_t current_vertex = face_vertices[j];
            current_vertex = vec3_rotate_x(current_vertex, mesh.rotation.x);
            current_vertex = vec3_rotate_y(current_vertex, mesh.rotation.y);
            current_vertex = vec3_rotate_z(current_vertex, mesh.rotation.z);

            transformed_vertices[j] = current_vertex;
        }
        //Cull back faces
        vec3_t cameraRay = vec3_sub(transformed_vertices[0],camera_pos);

        vec3_t v1 = vec3_sub(transformed_vertices[1],transformed_vertices[0]);
        vec3_t v2 = vec3_sub(transformed_vertices[2],transformed_vertices[0]);

        vec3_t tri_normal = vec3_cross(v1,v2);

        //vec3_normalize(&tri_normal);

        float orientation_from_camera = vec3_dot(cameraRay,tri_normal);

        if(orientation_from_camera >= 0){
            continue;
        }
        triangle_t projected_triangle;

        for(int j = 0; j < 3; j++){
            transformed_vertices[j].z -= camera_pos.z;
            
            vec2_t projected_vertex = project(transformed_vertices[j]);

            //scale and translate to middle of screen
            projected_vertex.x += window_width/2;
            projected_vertex.y += window_height/2;
            

            projected_triangle.points[j] = projected_vertex; 
        }
        array_push(triangles_to_render,projected_triangle);
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
            else if(event.key.keysym.sym == SDLK_UP) fov_factor += 10;
            else if(event.key.keysym.sym == SDLK_8) camera_pos.y += 1;
            else if(event.key.keysym.sym == SDLK_2) camera_pos.y -= 1;
            else if(event.key.keysym.sym == SDLK_4) camera_pos.x -= 1;
            else if(event.key.keysym.sym == SDLK_6) camera_pos.x += 1;
            else if(event.key.keysym.sym == SDLK_DOWN) fov_factor -= 10;
            break;        
    }
}

void render(void){
    //draw_grid();
  
    render_color_buffer();
    
    uint32_t col = 0xFFAB07E0;
    uint32_t col2 = 0xFF359DFA;
    //render_color_buffer();
    clear_color_buffer_gradient(col,col2);
    int num_triangles = array_length(triangles_to_render);
    for(int i = 0; i < num_triangles; i ++){
        triangle_t tri = triangles_to_render[i];
       
       // draw vertex points
        // drawRect(tri.points[0].x+camera_pos.x, tri.points[0].y+camera_pos.y, 10, 10, 0x000000);
        // drawRect(tri.points[1].x+camera_pos.x, tri.points[1].y+camera_pos.y, 10, 10, 0x000000);
        // drawRect(tri.points[2].x+camera_pos.x, tri.points[2].y+camera_pos.y, 10, 10, 0x000000);

       //draw face
        draw_filled_triangle(
            tri.points[0].x+camera_pos.x, tri.points[0].y+camera_pos.y,
            tri.points[1].x+camera_pos.x, tri.points[1].y+camera_pos.y,
            tri.points[2].x+camera_pos.x, tri.points[2].y+camera_pos.y
            ,0xFFFFFF);


         draw_triangle(
            tri.points[0].x+camera_pos.x, tri.points[0].y+camera_pos.y,
            tri.points[1].x+camera_pos.x, tri.points[1].y+camera_pos.y,
            tri.points[2].x+camera_pos.x, tri.points[2].y+camera_pos.y
            ,0x000000);


    //    / draw_filled_triangle(300,100,50,400,500,700,0xFFFFFF);
     
    }
    array_free(triangles_to_render);
  //  draw_grid();
    SDL_RenderPresent(renderer);
}


void free_resources(void){
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);

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



// int main(int argc, char* args[]){