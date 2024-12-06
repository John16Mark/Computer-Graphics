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
#include "structs/triangle.h"

typedef uint32_t color_t;
typedef vec2_t (*algoritmo_proyeccion)(vec3_t);

vec4_t *transformed_points;
vec2_t *projected_points;

vec3_t cube_rotation = {0, 0, 0};
vec3_t cube_translation = {0, 0, 0};
vec3_t cube_scale = {1, 1, 1};

int n_vertices;
int n_faces;

vec3_t camara = {0.0, 0.0, 0.0};

bool flag_perspective = true;
bool flag_vertices = true;
bool flag_lines = true;
bool flag_triangles = true;
bool flag_shader = false;

float *distancias;
float fov_factor = 420;
float fov_factor_perspective;
float fov_factor_orthographic;

bool is_running = false;
int previous_frame_time = 0;

light_t luz = {
    .direction = { 0, 0, 1 }
};


vec3_t calcular_normal_vertice(int vertice, index* caras_adyacentes, face_t* faces);
void calcular_normales_vertices(mesh_t* mesh, int n_faces);
float calcular_intensidad_lambert(vec3_t normal, vec3_t luz);
float calcular_intensidad(vec3_t normal, vec3_t light_dir);
void calcular_intensidades_vertices(mesh_t* mesh, vec3_t light_dir);

void setup(void) {
    // Allocate the requiered memory in bytes to hold the color buffer
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    
    if(!color_buffer) {fprintf(stderr, "Error allocating memory for frame buffer,\n");}
    // Creating a SDL texture that is used to display the color buffer
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

    load_cube_mesh_data();
    //load_obj_file_data("models/gunblade.obj");

    n_vertices = array_length(mesh.vertices);
    n_faces = array_length(mesh.faces);
    transformed_points = malloc(n_vertices * sizeof(vec4_t));
    projected_points = malloc(n_vertices * sizeof(vec2_t));

    //distancias = malloc(n_vertices * sizeof(float));
    float dist_larga = 0.0;
    for(int i=0; i<n_vertices; i++) {
        float dist_actual = vec3_length(mesh.vertices[i]);
        if (dist_actual > dist_larga) {
            dist_larga = dist_actual;
        }
    }
    fov_factor_perspective = fov_factor; // Donde fov_base es el que ya usas.
    fov_factor_orthographic = fov_factor / dist_larga;

    for(int i=0; i<n_faces; i++) {
        index* adyacentes_a = NULL;
        for(int j=0; j<n_faces; j++)
            // Si una cara diferente y también contiene el punto a
            if(i!=j && (mesh.faces[i].a == mesh.faces[j].a || mesh.faces[i].a == mesh.faces[j].b || mesh.faces[i].a == mesh.faces[j].c)) {
                index indice;
                indice.i = j;
                array_push(adyacentes_a, indice);
            }
        mesh.faces[i].a_adyacentes = adyacentes_a;

        index* adyacentes_b = NULL;
        for(int j=0; j<n_faces; j++)
            // Si una cara diferente y también contiene el punto b
            if(i!=j && (mesh.faces[i].b == mesh.faces[j].a || mesh.faces[i].b == mesh.faces[j].b || mesh.faces[i].b == mesh.faces[j].c)) {
                index indice;
                indice.i = j;
                array_push(adyacentes_b, indice);
            }
        mesh.faces[i].b_adyacentes = adyacentes_b;
        
        index* adyacentes_c = NULL;
        for(int j=0; j<n_faces; j++)
            // Si una cara diferente y también contiene el punto c
            if(i!=j && (mesh.faces[i].c == mesh.faces[j].a || mesh.faces[i].c == mesh.faces[j].b || mesh.faces[i].c == mesh.faces[j].c)) {
                index indice;
                indice.i = j;
                array_push(adyacentes_c, indice);
            }
        mesh.faces[i].c_adyacentes = adyacentes_c;
    }

    for(int i=0; i<n_faces; i++) {
        printf("------------------\nCara %d\n", i+1);
        printf("   a: \n");
        for(int j=0; j<array_length(mesh.faces[i].a_adyacentes); j++) {
            printf("      %d\n", (mesh.faces[i].a_adyacentes[j]).i+1);
        }
        printf("   b: \n");
        for(int j=0; j<array_length(mesh.faces[i].b_adyacentes); j++) {
            printf("      %d\n", (mesh.faces[i].b_adyacentes[j]).i+1);
        }
        printf("   c: \n");
        for(int j=0; j<array_length(mesh.faces[i].c_adyacentes); j++) {
            printf("      %d\n", (mesh.faces[i].c_adyacentes[j]).i+1);
        }
    }
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
            else if(event.key.keysym.sym == SDLK_s) {
                if(!flag_triangles) {
                    flag_triangles = true;
                }
                flag_shader = !flag_shader;
            }
            break;
    }
}

/*
* Function that recieves a 3D point and returns a projected ...
*/
vec2_t project_perspective(vec3_t point) {
    vec2_t res;
    res.x = (point.x*fov_factor_perspective)/point.z;
    res.y = (point.y*fov_factor_perspective)/point.z;
    return res;
}

vec2_t project_orthographic(vec3_t point) {
    vec2_t res;
    res.x = (point.x*fov_factor_orthographic);
    res.y = (point.y*fov_factor_orthographic);
    return res;
}

vec2_t project(vec3_t point, algoritmo_proyeccion funcion) {
    return funcion(point);
}

void update(void) {
    cube_rotation.x += 0.01;
    cube_rotation.y += 0.02;
    cube_rotation.z += 0.04;
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

    for(int i=0; i<n_vertices; i++) {
        // Transformar puntos
        vec4_t transformed_point = vec4_from_vec3(mesh.vertices[i]);
        transformed_point = mat4_mul_vec4(world_matrix, transformed_point);
        transformed_points[i] = transformed_point;
        // Proyectar
        vec2_t projected_point = (flag_perspective) ? project(vec3_from_vec4(transformed_points[i]), project_perspective) : project(vec3_from_vec4(transformed_points[i]), project_orthographic);
        projected_points[i] = projected_point;
    }

    // Determinar visibilidad de las caras
    for(int i=0; i<n_faces; i++) {
        face_t face = mesh.faces[i];
        vec3_t a_transformado = vec3_from_vec4(transformed_points[(face.a)-1]);
        vec3_t b_transformado = vec3_from_vec4(transformed_points[(face.b)-1]);
        vec3_t c_transformado = vec3_from_vec4(transformed_points[(face.c)-1]);
        vec3_t normal = vec3_normal(a_transformado, b_transformado, c_transformado);
        vec3_t camera_ray = vec3_sub(camara, a_transformado);
        float dot = vec3_dot(normal, camera_ray);
        mesh.faces[i].visible = dot >= 0;
        mesh.faces[i].normal = normal;
        
        // Gouraud Shading
        if(flag_shader) {
            calcular_normales_vertices(&mesh, n_faces);
        }

        // Calcular profundidad
        float x1 = a_transformado.x;    float x2 = b_transformado.x;    float x3 = c_transformado.x;
        float y1 = a_transformado.y;    float y2 = b_transformado.y;    float y3 = c_transformado.y;
        float z1 = a_transformado.z;    float z2 = b_transformado.z;    float z3 = c_transformado.z;
        float A = pow(x1-camara.x, 2) + pow(y1-camara.y, 2) + pow(z1-camara.z, 2);
        float B = pow(x2-camara.x, 2) + pow(y2-camara.y, 2) + pow(z2-camara.z, 2);
        float C = pow(x3-camara.x, 2) + pow(y3-camara.y, 2) + pow(z3-camara.z, 2);
        mesh.faces[i].depth = (sqrt(A)+sqrt(B)+sqrt(C))/3;
    }
    
    //QuickSort_faces(mesh.faces, 0, n_faces-1);
}

void render(void) {
    draw_grid();

    if(flag_triangles) {
        for(int i=0; i<n_faces; i++) {
            face_t face = mesh.faces[i];
            vec3_t a_transformado = vec3_from_vec4(transformed_points[(face.a)-1]);
            vec3_t b_transformado = vec3_from_vec4(transformed_points[(face.b)-1]);
            vec3_t c_transformado = vec3_from_vec4(transformed_points[(face.c)-1]);

            if (es_visible(a_transformado, b_transformado, c_transformado, camara) || !flag_perspective) {
                color_t color = mesh.faces[i].color;
                if (flag_shader) {/*
                    vec3_t normal = vec3_normal(a_transformado, b_transformado, c_transformado);
                    float I = - (vec3_dot(normal, luz.direction));
                    color = light_apply_intensity(WHITE, I);*/
                    float I_a = face.intensidad_a; // Calculadas previamente en la fase de actualización
                    float I_b = face.intensidad_b;
                    float I_c = face.intensidad_c;

                    render_triangle2(
                        (int)projected_points[(face.a)-1].x + window_width / 2,
                        (int)projected_points[(face.a)-1].y + window_height / 2,
                        I_a,
                        (int)projected_points[(face.b)-1].x + window_width / 2,
                        (int)projected_points[(face.b)-1].y + window_height / 2,
                        I_b,
                        (int)projected_points[(face.c)-1].x + window_width / 2,
                        (int)projected_points[(face.c)-1].y + window_height / 2,
                        I_c,
                        color, // Color base del triángulo
                        bresenham
                    );
                } else {
                    render_triangle(
                        (int)projected_points[(face.a)-1].x + window_width/2,
                        (int)projected_points[(face.a)-1].y + window_height/2,
                        (int)projected_points[(face.b)-1].x + window_width/2,
                        (int)projected_points[(face.b)-1].y + window_height/2,
                        (int)projected_points[(face.c)-1].x + window_width/2,
                        (int)projected_points[(face.c)-1].y + window_height/2,
                        color,
                        bresenham);
                }
                
                
                //printf("%d\n", i);
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

    free(transformed_points);
    free(projected_points);
    
    destroy_window();
    return 0;
}

vec3_t calcular_normal_vertice(int vertice, index* caras_adyacentes, face_t* faces) {
    vec3_t normal_sum = {0, 0, 0};
    int n = array_length(caras_adyacentes);
    // Sumatoria de las normales
    for (int i = 0; i < n; i++) {
        int face_index = caras_adyacentes[i].i;
        normal_sum = vec3_add(normal_sum, faces[face_index].normal);
    }
    // Promedio
    if (n > 0)
        normal_sum = vec3_div(normal_sum, n);
    // Normalizar el vector
    vec3_normalize(&normal_sum);
    return normal_sum;
}

void calcular_normales_vertices(mesh_t* mesh, int n_faces) {
    for (int i = 0; i < n_faces; i++) {
        face_t* face = &mesh->faces[i];

        // Calcular normales por vértice usando las caras adyacentes
        face->normal_a = calcular_normal_vertice(face->a, face->a_adyacentes, mesh->faces);
        face->normal_b = calcular_normal_vertice(face->b, face->b_adyacentes, mesh->faces);
        face->normal_c = calcular_normal_vertice(face->c, face->c_adyacentes, mesh->faces);
    }
}

float calcular_intensidad_lambert(vec3_t normal, vec3_t luz) {
    vec3_normalize(&luz);
    return fmax(0.0f, vec3_dot(normal, luz));
}

float calcular_intensidad(vec3_t normal, vec3_t light_dir) {
    vec3_normalize(&light_dir);
    float intensidad = vec3_dot(normal, light_dir);

    // Asegurarse de que la intensidad no sea negativa
    return intensidad > 0 ? intensidad : 0;
}

void calcular_intensidades_vertices(mesh_t* mesh, vec3_t light_dir) {
    for (int i = 0; i < array_length(mesh->faces); i++) {
        face_t* face = &mesh->faces[i];

        // Calcular intensidad en cada vértice
        face->intensidad_a = calcular_intensidad(face->normal_a, light_dir);
        face->intensidad_b = calcular_intensidad(face->normal_b, light_dir);
        face->intensidad_c = calcular_intensidad(face->normal_c, light_dir);
    }
}
/*
void rasterizar_triangulo(face_t* face, framebuffer_t* framebuffer) {
    // Interpolar las intensidades de los vértices A, B y C
    for (int y = min_y; y < max_y; y++) {
        for (int x = min_x; x < max_x; x++) {
            if (dentro_del_triangulo(x, y, face)) {
                float bary_a, bary_b, bary_c;
                calcular_barycentric(x, y, face, &bary_a, &bary_b, &bary_c);

                // Interpolar la intensidad
                float intensidad = face->intensidad_a * bary_a +
                                   face->intensidad_b * bary_b +
                                   face->intensidad_c * bary_c;

                // Establecer el color final
                color_t color = face->color;
                color.r *= intensidad;
                color.g *= intensidad;
                color.b *= intensidad;

                framebuffer_set_pixel(framebuffer, x, y, color);
            }
        }
    }
}*/
