#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "structs/display.h"

bool is_running = false;
int previous_frame_time = 0;

void setup(void) {
    // Allocate the requiered memory in bytes to hold the color buffer
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    
    if(!color_buffer) {fprintf(stderr, "Error allocating memory for frame buffer,\n");}
    // Creating a SDL texture that is used to display the color buffer
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
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

void update(void) {

}

void render(void) {
    draw_grid();

    draw_rect(100, 100, 300, 150, 0xFFFF00FF);

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
    
    destroy_window();
    return 0;
}

