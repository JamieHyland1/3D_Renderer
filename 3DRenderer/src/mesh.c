#include <stdio.h>
#include  <unistd.h>
#include "array.h"
#include "mesh.h"

mesh_t mesh = {
  .vertices =NULL,
  .faces = NULL,
  .rotation = {0,0,0},
  .scale = {1.0,1.0,1.0},
  .translation = {0,0,0}
};

 vec3_t cube_vertices[N_CUBE_VERTICES] = {
    { .x = -1, .y = -1, .z = -1 }, // 1
    { .x = -1, .y =  1, .z = -1 }, // 2
    { .x =  1, .y =  1, .z = -1 }, // 3
    { .x =  1, .y = -1, .z = -1 }, // 4
    { .x =  1, .y =  1, .z =  1 }, // 5
    { .x =  1, .y = -1, .z =  1 }, // 6
    { .x = -1, .y =  1, .z =  1 }, // 7
    { .x = -1, .y = -1, .z =  1 }  // 8
};

face_t cube_faces[N_CUBE_FACES] = {
      // front
    { .a = 1, .b = 2, .c = 3 },
    { .a = 1, .b = 3, .c = 4 },
    // right
    { .a = 4, .b = 3, .c = 5 },
    { .a = 4, .b = 5, .c = 6 },
    // back
    { .a = 6, .b = 5, .c = 7 },
    { .a = 6, .b = 7, .c = 8 },
    // left
    { .a = 8, .b = 7, .c = 2 },
    { .a = 8, .b = 2, .c = 1 },
    // top
    { .a = 2, .b = 7, .c = 5 },
    { .a = 2, .b = 5, .c = 3 },
    // bottom
    { .a = 6, .b = 8, .c = 1 },
    { .a = 6, .b = 1, .c = 4 }
};

void load_cube_mesh_data(void){
    for(int i = 0; i < N_CUBE_VERTICES; i++){
      vec3_t cube_vertex = cube_vertices[i];
      array_push(mesh.vertices,cube_vertex);
    }
    for(int i = 0; i < N_CUBE_FACES; i++){
      face_t cube_face = cube_faces[i];
      array_push(mesh.faces,cube_face);
    }
}

void load_obj_file_data(char* filename){
    // TODO read contents of file
    // populate mesh struct with the vertices and faces
    FILE *file = fopen(filename,"r");
    if(file == NULL){
      printf("no file found");
      exit(-1);
    }
      do {
     
        char data[50];

        fgets(data,50,file);
        if(data[0] == 'v' ){

          vec3_t vertex;


          strtok(data," ");
          char* x =  strtok(NULL," ");
          vertex.x = atof(x);
          //printf("vertex x: %s\n",x);
          char* y =  strtok(NULL," ");
          vertex.y = atof(y);
          //printf("vertex y: %s\n",y);
          char* z =  strtok(NULL," ");
          vertex.z = atof(z);

        //  printf("vertex z: %s\n", z);
          array_push(mesh.vertices,vertex);
         // printf("the length of the array is: %d\n", array_length(mesh.vertices));
          
        }
        if(data[0] == 'f'){
            strtok(data," ");
            face_t face;
           
            char* c1 = strtok(NULL," ");
            char* c2 = strtok(NULL," ");
            char* c3 = strtok(NULL," ");

            face.a = atoi(c1);
            face.b = atoi(c2);
            face.c = atoi(c3);
            
            //printf("Each face: %d %d %d\n", face.a,face.b,face.c);

            array_push(mesh.faces,face);

        }


   
    } while (!feof(file));
 
    // Closing the file
    fclose(file);
}

//  void load_obj_file_data2(char* filename){
// FILE* file;
//     file = fopen(filename, "r");
//     char line[1024];

//     while (fgets(line, 1024, file)) {
//         // Vertex information
//         if (strncmp(line, "v ", 2) == 0) {
//             vec3_t vertex;
//             sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
//             array_push(mesh.vertices, vertex);
//         }
//         // Face information
//         if (strncmp(line, "f ", 2) == 0) {
//             int vertex_indices[3];
//             int texture_indices[3];
//             int normal_indices[3];
//             sscanf(
//                 line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
//                 &vertex_indices[0], &texture_indices[0], &normal_indices[0], 
//                 &vertex_indices[1], &texture_indices[1], &normal_indices[1], 
//                 &vertex_indices[2], &texture_indices[2], &normal_indices[2]
//             ); 
//             face_t face = {
//                 .a = vertex_indices[0],
//                 .b = vertex_indices[1],
//                 .c = vertex_indices[2]
//             };
//             array_push(mesh.faces, face);
//         }
//     }
// }





  // if(ch=='v' && ignoreLine == false){
        //   ch = fgetc(file);
        //   readingVertices = true;
        //   vec3_t vertex;
        //   char* line = NULL;
        //   do{
        //     array_push(line,ch);
        //     ch = fgetc(file);
            

        //   }while(ch != '\n');
           
        //    sscanf(line,"%f;%f;%f", &vertex.x,&vertex.y,&vertex.z);
        //    printf("value of vertex %c", *line[2]);
        //    array_free(line);
        // } 
     // if(ch == '\n'){
        //   ignoreLine      = false;
        //   readingVertices = false;
        //   readingFaces    = false;

        // }
 
        // Checking if character is not EOF.
        // If it is EOF stop eading.