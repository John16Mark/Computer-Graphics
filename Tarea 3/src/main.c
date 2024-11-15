#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "structs/display.h"
#include "structs/vector.h"
#include "structs/array.h"

bool is_running = false;
int previous_frame_time = 0;

vec2_t *poligono;
vec2_t *poligono_ordenado;
int n_poligono = 0;

void quicksort_elementos(vec2_t *v, int p, int r);
int pivot_elementos(vec2_t *v, int p, int r);
void intercambiar_elementos(vec2_t *v, int i, int j);
bool condicion(vec2_t i, vec2_t j);

void setup(void) {
    // Allocate the requiered memory in bytes to hold the color buffer
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    
    if(!color_buffer) {fprintf(stderr, "Error allocating memory for frame buffer,\n");}
    // Creating a SDL texture that is used to display the color buffer
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

    vec2_t v = {100, 350};
    array_push(poligono, v);
    v.x = 200; v.y = 600;
    array_push(poligono, v);
    v.x = 400; v.y = 750;
    array_push(poligono, v);
    v.x = 800; v.y = 450;
    array_push(poligono, v);
    v.x = 550; v.y = 250;
    array_push(poligono, v);
    v.x = 400; v.y = 350;
    array_push(poligono, v);
    v.x = 250; v.y = 250;
    array_push(poligono, v);

    n_poligono = array_length(poligono);
    poligono_ordenado = malloc(n_poligono*sizeof(vec2_t));
    for(int i=0; i<n_poligono; i++) {
        poligono_ordenado[i] = poligono[i];
    }
    quicksort_elementos(poligono_ordenado, 0, n_poligono-1);
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                is_running = false;
            break;
    }
}

void update(void) { // línea 82

}

void render(void) {
    draw_grid();

    // Dibujar relleno del polígono
    scanline_fill(poligono, n_poligono, 0xFFEEEEEE);
    for(int i=0; i<n_poligono; i++) // Dibujar aristas del polígono
        draw_line(
            poligono[i].x, poligono[i].y,
            poligono[(i+1)%n_poligono].x, poligono[(i+1)%n_poligono].y,
            0xFF40FFFF,
            bresenham);
    for(int i=0; i<n_poligono; i++) // Dibujar vértices del polígono
        draw_rect(poligono[i].x, poligono[i].y, 4, 4, 0xFFFF00FF);

/*
    // Dibujar relleno del polígono
    scanline_fill(poligono_ordenado, n_poligono, 0xFFEEEEEE);
    for(int i=0; i<n_poligono; i++) // Dibujar aristas del polígono
        draw_line(
            poligono_ordenado[i].x, poligono_ordenado[i].y,
            poligono_ordenado[(i+1)%n_poligono].x, poligono_ordenado[(i+1)%n_poligono].y,
            0xFF40FFFF,
            bresenham);
    for(int i=0; i<n_poligono; i++) // Dibujar vértices del polígono
        draw_rect(poligono_ordenado[i].x, poligono_ordenado[i].y, 4, 4, 0xFFFF00FF);
*/
    render_color_buffer();
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    is_running = initialize_window();

    setup();

    while(is_running) {
        
        previous_frame_time = SDL_GetTicks();
        
        process_input();
        update();
        render();

        // Guarantees that every frame is executed every FRAME_TARGET_TIME
        // Waite some time until the reach the target frame time un milliseconds
        int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

        // delay execution if we are running too fast
        if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
            SDL_Delay(time_to_wait);
        }

    }

    array_free(poligono);
    free(poligono_ordenado);

    destroy_window();
    return 0;
}

void quicksort_elementos(vec2_t *v, int p, int r) {
    if (p < r) {
        int j = pivot_elementos(v, p, r);
        quicksort_elementos(v, p, j-1);
        quicksort_elementos(v, j+1, r);
    }
}

int pivot_elementos(vec2_t *v, int p, int r) {
    vec2_t piv = v[p];
    int i = p+1, j = r;
    do {
        while (condicion(v[i], piv)/*v[i].y <= piv.y*/ && i < j) i++;
        while (!condicion(v[j], piv)/*v[j].y > piv.y*/) j--;
        if (i < j)
            intercambiar_elementos(v, i, j);
    } while (i < j);
    intercambiar_elementos(v, p, j);
    return j;
}

void intercambiar_elementos(vec2_t *v, int i, int j) {
    vec2_t temp = v[j];
    v[j] = v[i];
    v[i] = temp;
}

bool condicion(vec2_t i, vec2_t j) {
    if(i.y < j.y)
        return true;
    if(i.y > j.y)
        return false;
    if(i.x <= j.x)
        return true;
    return false;
}