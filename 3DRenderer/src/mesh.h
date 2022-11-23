#ifndef mesh_h
#define mesh_h

#include "triangle.h"
#include "vector.h"


#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6*2)


extern vec3_t mesh_vertices[N_CUBE_VERTICES];
extern face_t mesh_faces[N_CUBE_FACES];
/////////////////////////////////////////////////////////////////////////////////////////
// Define a struct for dynamic size meshes
////////////////////////////////////////////////////////////////////////////////////////
typedef struct {
    vec3_t* vertices; //dynamic array
    face_t* faces; //dynamic array
    vec3_t rotation;
} mesh_t;

extern mesh_t mesh;


void load_cube_mesh_data(void);

#endif