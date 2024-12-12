#ifndef INC_3DRENDERER_TRIANGLE_H
#define INC_3DRENDERER_TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "display.h"

typedef struct {
    int i;
} index;

typedef struct {
    int a;
    int b;
    int c;
    color_t color;
    bool visible;
    float depth;

    vec3_t normal;         // Normal de la cara
    vec3_t normal_a;       // Normal en el vértice A
    vec3_t normal_b;       // Normal en el vértice B
    vec3_t normal_c;       // Normal en el vértice C

    float intensidad_a;    // Intensidad en el vértice A
    float intensidad_b;    // Intensidad en el vértice B
    float intensidad_c;    // Intensidad en el vértice C

    index* a_adyacentes;   // Índices de caras adyacentes al vértice A
    index* b_adyacentes;   // Índices de caras adyacentes al vértice B
    index* c_adyacentes;   // Índices de caras adyacentes al vértice C
} face_t;;

typedef struct {
    vec2_t points[3];
} triangle_t;

typedef struct {
    vec3_t direction;
    float ambient;
} light_t;

extern light_t luz;
extern bool debug;

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color, algoritmo_lineas funcion);
void QuickSort_faces(face_t *A, int p, int r);
int Pivot_faces(face_t *A, int p, int r);
void Intercambiar_faces(face_t *A, int i, int j);
color_t light_apply_intensity(color_t original_color, float percentage_factor);
void render_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color, algoritmo_lineas funcion);
void render_flat_bottom(int x0, int y0, int x1, int y1, int x2, int y2, color_t color, algoritmo_lineas funcion);
void render_flat_top(int x0, int y0, int x1, int y1, int x2, int y2, color_t color, algoritmo_lineas funcion);

void render_triangle2(
    int x0, int y0, float i0,
    int x1, int y1, float i1,
    int x2, int y2, float i2,
    color_t base_color,
    algoritmo_lineas funcion
);
void render_flat_bottom2(
    int x0, int y0, float i0,
    int x1, int y1, float i1,
    int x2, int y2, float i2,
    color_t base_color,
    algoritmo_lineas funcion
);
void render_flat_top2(
    int x0, int y0, float i0,
    int x1, int y1, float i1,
    int x2, int y2, float i2,
    color_t base_color,
    algoritmo_lineas funcion
);

#endif //INC_3DRENDERER_TRIANGLE_H
