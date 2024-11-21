#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "structs/display.h"
#include "structs/vector.h"
#include "structs/matrix.h"
#include "structs/mesh.h"
#include "structs/array.h"

typedef uint32_t color_t;
typedef vec2_t (*algoritmo_proyeccion)(vec3_t);

vec2_t *projected_points;

vec3_t cube_rotation = {0, 0, 0};
vec3_t cube_translation = {0, 0, 0};
vec3_t cube_scale = {1, 1, 1};

int n_vertices;
int n_faces;

vec3_t camera_position = {0,0,0};

bool flag_perspective = true;
bool flag_vertices = true;
bool flag_lines = true;
bool flag_triangles = true;

float fov_factor = 360;

bool is_running = false;
int previous_frame_time = 0;

void setup(void) {
    // Allocate the requiered memory in bytes to hold the color buffer
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    
    if(!color_buffer) {fprintf(stderr, "Error allocating memory for frame buffer,\n");}
    // Creating a SDL texture that is used to display the color buffer
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

    load_cube_mesh_data();
    //load_obj_file_data("models/cube.obj");

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
            else if(event.key.keysym.sym == SDLK_p)
                flag_perspective = !flag_perspective;
            
            else if(event.key.keysym.sym == SDLK_v) {
                if(flag_vertices && !flag_lines && !flag_triangles) {
                    flag_lines = true;
                }
                flag_vertices = !flag_vertices;
            }
            else if(event.key.keysym.sym == SDLK_l) {
                if(flag_lines && !flag_vertices && !flag_triangles) {
                    flag_vertices = true;
                }
                flag_lines = !flag_lines;
            }
            else if(event.key.keysym.sym == SDLK_f) {
                if(flag_triangles && !flag_vertices && !flag_lines) {
                    flag_vertices = true;
                }
                flag_triangles = !flag_triangles;
            }
            break;
    }
}

/*
* Function that recieves a 3D point and returns a projected ...
*/
vec2_t project_perspective(vec3_t point) {
    vec2_t res;
    res.x = (point.x*fov_factor)/point.z;
    res.y = (point.y*fov_factor)/point.z;
    return res;
}

vec2_t project_orthographic(vec3_t point) {
    vec2_t res;
    res.x = (point.x*fov_factor);
    res.y = (point.y*fov_factor);
    return res;
}

vec2_t project(vec3_t point, algoritmo_proyeccion funcion) {
    return funcion(point);
}

float calculate_area_2d(vec2_t p1, vec2_t p2, vec2_t p3) {
    return 0.5f * (p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y));
}

void ensure_counter_clockwise(face_t* face, vec3_t* vertices) {
    vec3_t a = vertices[face->a];
    vec3_t b = vertices[face->b];
    vec3_t c = vertices[face->c];

    vec2_t pa = project(a, project_perspective);
    vec2_t pb = project(b, project_perspective);
    vec2_t pc = project(c, project_perspective);

    if (calculate_area_2d(pa, pb, pc) < 0) {
        // Intercambiar b y c para corregir el orden
        int temp = face->b;
        face->b = face->c;
        face->c = temp;
    }
}

void update(void) {
    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;
    /*
    cube_rotation.x = 1.6;
    cube_rotation.y = 1.6;
    cube_rotation.z = 1.6;
    */
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
        // Transformar puntos
        vec4_t transformed_point = vec4_from_vec3(mesh.vertices[i]);
        transformed_point = mat4_mul_vec4(world_matrix, transformed_point);
        transformed_points[i] = transformed_point;
        // Proyectar
        vec2_t projected_point = (flag_perspective) ? project(vec3_from_vec4(transformed_points[i]), project_perspective) : project(vec3_from_vec4(transformed_points[i]), project_orthographic);
        projected_points[i] = projected_point;
    }
    free(transformed_points);
}

void render(void) {
    draw_grid();

    if(flag_triangles) {
        for(int i=0; i<n_faces; i++) {
            face_t face = mesh.faces[i];
            ensure_counter_clockwise(&face, mesh.vertices);
            vec3_t a = mesh.vertices[face.a-1];
            vec3_t b = mesh.vertices[face.b-1];
            vec3_t c = mesh.vertices[face.c-1];

            if (es_visible(a, b, c, camera_position)) {
                render_triangle(
                    (int)projected_points[(mesh.faces[i].a)-1].x + window_width/2,
                    (int)projected_points[(mesh.faces[i].a)-1].y + window_height/2,
                    (int)projected_points[(mesh.faces[i].b)-1].x + window_width/2,
                    (int)projected_points[(mesh.faces[i].b)-1].y + window_height/2,
                    (int)projected_points[(mesh.faces[i].c)-1].x + window_width/2,
                    (int)projected_points[(mesh.faces[i].c)-1].y + window_height/2,
                    mesh.faces[i].color,
                    //0xFFFFFFFF,
                    bresenham);
            }
        }
    }

    if(flag_lines) {
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
    }

    if(flag_vertices) {
        for(int i=0; i<n_vertices; i++) {
            vec2_t projected_point = projected_points[i];
            draw_rect(
                projected_point.x + window_width/2,
                projected_point.y + window_height/2,
                4,
                4,
                0xFFFF00
            );
        }
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

