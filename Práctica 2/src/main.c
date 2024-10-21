#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "structs/display.h"
#include "structs/vector.h"
#include "structs/matrix.h"
#include "structs/mesh.h"
#include "structs/array.h"

vec2_t *projected_points;

vec3_t cube_rotation = {0, 0, 0};
vec3_t cube_translation = {0, 0, 0};
vec3_t cube_scale = {1, 1, 1};

int n_vertices;
int n_faces;

float fov_factor = 720;

bool is_running = false;
int previous_frame_time = 0;

void setup(void) {
    // Allocate the requiered memory in bytes to hold the color buffer
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    
    if(!color_buffer) {fprintf(stderr, "Error allocating memory for frame buffer,\n");}
    // Creating a SDL texture that is used to display the color buffer
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

    //load_cube_mesh_data();
    load_obj_file_data("models/cubo.obj");

    n_vertices = array_length(mesh.vertices);
    n_faces = array_length(mesh.faces);
    projected_points = malloc(n_vertices * sizeof(vec2_t));
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

 /*
 * Function that recieves a 3D point and returns a projected ... // TODO implementar las fórmulas para la proyección las dos, pero solo usar una
 */
vec2_t project_perspective(vec3_t point) {
    vec2_t res;
    res.x = (point.x*fov_factor)/point.z;
    res.y = (point.y*fov_factor)/point.z;
    return res;
}

void update(void) { // línea 82
    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;

    //cube_rotation.y = 1;

    //cube_rotation.z = 0.5;

    //cube_translation.x = -100.0;
    cube_translation.z = 5.0;

    // Create scale, rotation and translation matrices that will ve used to multiply the Mesh
    mat4_t scale_matrix = mat4_make_scale(cube_scale.x,
                                            cube_scale.y,
                                            cube_scale.x);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(cube_rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(cube_rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(cube_rotation.z);
    mat4_t translation_matrix = mat4_make_translation(
        cube_translation.x, cube_translation.y,
        cube_translation.z);

    // Create a World Matrix combining scale, rotation and translation matrices
    mat4_t world_matrix = mat4_identity();

    // Order matters: First scale, then rotate, then translate.
    // [T]*[R]*[S]*v
    world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
    world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

    vec4_t *transformed_points = malloc(n_vertices * sizeof(vec4_t));
    for(int i=0; i<n_vertices; i++) {
        vec4_t transformed_point = vec4_from_vec3(mesh.vertices[i]);
        transformed_point = mat4_mul_vec4(world_matrix, transformed_point);
        transformed_points[i] = transformed_point;
        vec2_t projected_point = project_perspective(vec3_from_vec4(transformed_points[i]));
        projected_points[i] = projected_point;
    }
    free(transformed_points);
}

void render(void) {
    draw_grid();

    for(int i=0; i<n_vertices; i++) {
        vec2_t projected_point = projected_points[i];
        draw_rect(
            projected_point.x + window_width/2,
            projected_point.y + window_height/2,
            4,
            4,
            0xFFFFFF00
        );
    }

    for(int i=0; i<n_faces; i++) {
        draw_triangle(
            (int)projected_points[(int)(mesh.faces[i].a)-1].x + window_width/2,
            (int)projected_points[(int)(mesh.faces[i].a)-1].y + window_height/2,
            (int)projected_points[(int)(mesh.faces[i].b)-1].x + window_width/2,
            (int)projected_points[(int)(mesh.faces[i].b)-1].y + window_height/2,
            (int)projected_points[(int)(mesh.faces[i].c)-1].x + window_width/2,
            (int)projected_points[(int)(mesh.faces[i].c)-1].y + window_height/2,
            0xFFFFFFFF, bresenham);
    }

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

    free(projected_points);
    
    destroy_window();
    return 0;
}

