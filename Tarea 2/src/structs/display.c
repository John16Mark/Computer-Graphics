#include "display.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;
uint32_t* color_buffer = NULL; // Some books like to call this as "frame buffer." For all practical purposes, color buffer & frame buffer are the same thing; they are a mirror in memory of the pixels that we want to see in our display.
int window_width = 0;
int window_height = 0;

bool initialize_window(void){
    if(SDL_Init(SDL_INIT_EVERYTHING) !=0){
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // Use SDL to query what is the fullscreen maximum width and height
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(
            0, // the device
            &displayMode
    );
    window_width = displayMode.w; //800;
    window_height = displayMode.h; //600;

    //Create a SDL window
    window = SDL_CreateWindow(
            NULL, // title
            SDL_WINDOWPOS_CENTERED, //pos x
            SDL_WINDOWPOS_CENTERED, //pos y
            window_width, // width
            window_height, // height
            SDL_WINDOW_BORDERLESS
    );

    if(!window){
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    //Create a SDL renderer
    renderer = SDL_CreateRenderer(
            window,
            -1,
            0
    );
    if(!renderer){
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }
    // Change the video mode to be real fullscreen, no "fake full screen"
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}

void draw_grid(void) {
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            if (x % 50 == 0 || y % 50 == 0) {
                color_buffer[(window_width * y) + x] = 0xFF444444;
            }
        }
    }
}

/*void draw_grid(void) {
    for (int y = 0; y < window_height; y += 10) {
        for (int x = 0; x < window_width; x += 10) {
            color_buffer[(window_width * y) + x] = 0xFF444444;
        }
    }
}*/

void draw_pixel(int x, int y, color_t color) {
    if(x>=0 && x<window_width && y>=0 && y<window_height){
        color_buffer[(window_width * y) + x] = color;
    }
}

void draw_line(int x0, int y0, int x1, int y1, color_t color, algoritmo_lineas funcion) {
    funcion(x0, y0, x1, y1, color);
}

void bresenham(int x0, int y0, int x1, int y1, color_t color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    // incrementos
    int xinc = (x0 < x1) ? 1 : -1; 
    int yinc = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += xinc;
        }
        if (e2 < dx) {
            err += dx;
            y0 += yinc;
        }
    }
}

void DDA(int x0, int y0, int x1, int y1, color_t color) 
{ 
    int dx = x1 - x0; 
    int dy = y1 - y0; 
    int dmayor = abs(dx) > abs(dy) ? abs(dx) : abs(dy); 
    // incrementos
    float xinc = dx / (float)dmayor; 
    float yinc = dy / (float)dmayor; 
    float x = x0; 
    float y = y0; 
    for (int i = 0; i <= dmayor; i++) { 
        draw_pixel(x0, y0, color);
        x += xinc;
        y += yinc;
    } 
} 

void draw_rect(int x, int y, int width, int height, color_t color) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int current_x = x + i;
            int current_y = y + j;
            draw_pixel(current_x, current_y, color);
            // color_buffer[(window_width * current_y) + current_x] = color;
        }
    }
}

void draw_elipse(int rx, int ry, int cx, int cy, color_t color) {
    float x = 0;
    float y = ry;

    float dx = 2*ry*ry*x;
    float dy = 2*rx*rx*y;

    // R1
    float p = (float)(ry*ry) + 0.25*(float)(rx*rx) - (float)(rx*rx*ry);
    while (dy > dx) {
        draw_pixel(cx+(int)x, cy+(int)y, color);
        draw_pixel(cx+(int)-x, cy+(int)y, color);
        draw_pixel(cx+(int)x, cy+(int)-y, color);
        draw_pixel(cx+(int)-x, cy+(int)-y, color);
 
        if (p < 0) {
            x++;
            dx = 2*ry*ry*x;
            dy = 2*rx*rx*y;
            p += dx + (ry*ry);
        }
        else {
            x++;
            y--;
            dx = 2*ry*ry*x;
            dy = 2*rx*rx*y;
            p += dx - dy + (ry*ry);
        }
    }

    // R2
    p = ry*ry*(x+0.5)*(x+0.5) + rx*rx*(y-1)*(y-1) - rx*rx*ry*ry;
    while (y >= 0) {
        draw_pixel(cx+(int)x, cy+(int)y, color);
        draw_pixel(cx+(int)-x, cy+(int)y, color);
        draw_pixel(cx+(int)x, cy+(int)-y, color);
        draw_pixel(cx+(int)-x, cy+(int)-y, color);
 
        if (p > 0) {
            y--;
            dx = 2*ry*ry*x;
            dy = 2*rx*rx*y;
            p += (rx*rx) - dy;
        }
        else {
            y--;
            x++;
            dx = 2*ry*ry*x;
            dy = 2*rx*rx*y;
            p += dx - dy + (rx*rx);
        }
    }
}

void render_color_buffer(void){
    // Copy the frame buffer to the texture
    SDL_UpdateTexture(
            color_buffer_texture,
            NULL, // No rectangle area to update. NULL indicates that update the entire texture
            color_buffer, // the source, raw pixel data
            (int)(window_width * sizeof(uint32_t)) // The number of bytes in a row of pixel data
    );

    // Copy the texture (or portion) to the current rendering target
    SDL_RenderCopy(renderer,
                   color_buffer_texture,
                   NULL, // render the entire texture, no specific rectangle
                   NULL // the entire rendering target, no specific rectangle (section)
    );
}

void clear_color_buffer(color_t color){
    for(int i = 0; i < window_width * window_height; i++){
        color_buffer[i] = color;
    }

}

void destroy_window(void){
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}