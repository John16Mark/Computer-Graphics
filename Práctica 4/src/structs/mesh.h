#ifndef INC_3DRENDERER_MESH_H
#define INC_3DRENDERER_MESH_H

#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (6 * 2) // 6 cube faces, 2 triangles per face

#define RED     0xFFFF0000
#define GREEN   0xFF00FF00
#define BLUE    0xFF0000FF
#define CYAN    0xFF00FFFF
#define MAGENTA 0xFFFF00FF
#define YELLOW  0xFFFFFF00
#define WHITE   0xFFFFFFFF
#define ORANGE  0xFFFF6A00

extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];

// Define a struct for dynamic size meshes, with array of vertices and faces
typedef struct {
    vec3_t* vertices;   // dynamic array of vertices
    face_t* faces;      // dynamic array of faces
} mesh_t;

extern mesh_t mesh;

void load_cube_mesh_data(void);
void load_obj_file_data(char* filename);

#endif //INC_3DRENDERER_MESH_H
