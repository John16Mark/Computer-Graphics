#include "display.h"
#include "triangle.h"

// Draw a triangle using three raw line calls
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color, algoritmo_lineas funcion) {
    draw_line(x0, y0, x1, y1, color, funcion);
    draw_line(x1, y1, x2, y2, color, funcion);
    draw_line(x2, y2, x0, y0, color, funcion);
}

bool comparar_vec2_t(vec2_t p1, vec2_t p2) {
    if(p1.y < p2.y) {
        return false;
    } else if(p1.y > p2.y) {
        return true;
    } else {
        if(p1.x < p2.x) {
            return false;
        } else {
            return true;
        }
    }
}

void render_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color, algoritmo_lineas funcion) {
    vec2_t p0 = {x0, y0};
    vec2_t p1 = {x1, y1};
    vec2_t p2 = {x2, y2};
    vec2_t m;

    if(comparar_vec2_t(p0, p2)) {
        m = p0;
        p0 = p2;
        p2 = m;
    }
    if(comparar_vec2_t(p0, p1)) {
        m = p0;
        p0 = p1;
        p1 = m;
    }
    if(comparar_vec2_t(p1, p2)) {
        m = p1;
        p1 = p2;
        p2 = m;
    }

    if(p2.y - p0.y == 0)
        return;
    
    m.y = p1.y;
    m.x = (((p2.x - p0.x)*(p1.y - p0.y))/(p2.y - p0.y)) + p0.x;
    /*
    draw_rect(p0.x, p0.y, 5, 5, 0xFFFFFF00); // Amarillo
    draw_rect(p1.x, p1.y, 5, 5, 0xFF00FFFF); // Cian
    draw_rect(p2.x, p2.y, 5, 5, 0xFFFF00FF); // Magenta
    draw_rect(m.x, m.y, 5, 5, 0xFFFF0000);
    */
    if(p0.y != p1.y)
        render_flat_bottom(p0.x, p0.y, p1.x, p1.y, m.x, m.y, color, funcion);
    if(p1.y != p2.y)
        render_flat_top(p1.x, p1.y, m.x, m.y, p2.x, p2.y, color, funcion);
}

void render_flat_bottom(int x0, int y0, int x1, int y1, int x2, int y2, color_t color, algoritmo_lineas funcion) {
    float m1 = ((float)x1 - (float)x0)/((float)y1 - (float)y0);
    float m2 = ((float)x2 - (float)x0)/((float)y2 - (float)y0);
    float xstart = x0;
    float xend = x0;
    for(int y=y0; y <= y1; y++) {
        draw_line((int)xstart, y, (int)xend, y, color, funcion);
        xstart += m1;
        xend += m2;
    }
}

void render_flat_top(int x0, int y0, int x1, int y1, int x2, int y2, color_t color, algoritmo_lineas funcion) {
    float m1 = ((float)x2 - (float)x0)/((float)y2 - (float)y0);
    float m2 = ((float)x2 - (float)x1)/((float)y2 - (float)y0);
    float xstart = x2;
    float xend = x2;
    for(int y=y2; y>y0; y--) {
        draw_line((int)xstart, y, (int)xend, y, color, funcion);
        xstart -= m1;
        xend -= m2;
    }
}
