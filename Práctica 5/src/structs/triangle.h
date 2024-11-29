#ifndef INC_3DRENDERER_TRIANGLE_H
#define INC_3DRENDERER_TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "display.h"

typedef struct {
    int a;
    int b;
    int c;
    color_t color;
    bool visible;
    float depth;
} face_t;

typedef struct {
    vec2_t points[3];
} triangle_t;

typedef struct {
    vec3_t direction;
} light_t;

extern light_t luz;

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color, algoritmo_lineas funcion);
void QuickSort_faces(face_t *A, int p, int r);
int Pivot_faces(face_t *A, int p, int r);
void Intercambiar_faces(face_t *A, int i, int j);
color_t light_apply_intensity(color_t original_color, float percentage_factor);
void render_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color, algoritmo_lineas funcion);
void render_flat_bottom(int x0, int y0, int x1, int y1, int x2, int y2, color_t color, algoritmo_lineas funcion);
void render_flat_top(int x0, int y0, int x1, int y1, int x2, int y2, color_t color, algoritmo_lineas funcion);


#endif //INC_3DRENDERER_TRIANGLE_H
