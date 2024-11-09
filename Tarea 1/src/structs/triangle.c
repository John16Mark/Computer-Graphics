#include "display.h"
#include "triangle.h"

// Draw a triangle using three raw line calls
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color, algoritmo_lineas funcion) {
    draw_line(x0, y0, x1, y1, color, funcion);
    draw_line(x1, y1, x2, y2, color, funcion);
    draw_line(x2, y2, x0, y0, color, funcion);
}