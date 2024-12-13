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
bool flag_vertices = false;
bool flag_lines = false;
bool flag_triangles = true;
bool flag_shader = false;

float *distancias;
float fov_factor = 420;
float fov_factor_perspective;
float fov_factor_orthographic;

bool is_running = false;
int previous_frame_time = 0;

bool debug = false;
bool debug2 = false;
bool debug3 = false;

float angulo_luz = 0.0f;
const float delta_angulo_luz = 0.03f; // Rota suavemente
light_t luz = {
    .direction = { 1, 0, 0 },
    .ambient = 0.1
};

vec3_t calcular_normal_vertice(index* caras_adyacentes);

void setup(void) {
    // Allocate the requiered memory in bytes to hold the color buffer
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    
    if(!color_buffer) {fprintf(stderr, "Error allocating memory for frame buffer,\n");}
    // Creating a SDL texture that is used to display the color buffer
    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);

    //load_cube_mesh_data();
    load_obj_file_data("models/Sword.obj");

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
            if((mesh.faces[i].a == mesh.faces[j].a || mesh.faces[i].a == mesh.faces[j].b || mesh.faces[i].a == mesh.faces[j].c)) {
                index indice;
                indice.i = j;
                array_push(adyacentes_a, indice);
            }
        mesh.faces[i].a_adyacentes = adyacentes_a;

        index* adyacentes_b = NULL;
        for(int j=0; j<n_faces; j++)
            // Si una cara diferente y también contiene el punto b
            if((mesh.faces[i].b == mesh.faces[j].a || mesh.faces[i].b == mesh.faces[j].b || mesh.faces[i].b == mesh.faces[j].c)) {
                index indice;
                indice.i = j;
                array_push(adyacentes_b, indice);
            }
        mesh.faces[i].b_adyacentes = adyacentes_b;
        
        index* adyacentes_c = NULL;
        for(int j=0; j<n_faces; j++)
            // Si una cara diferente y también contiene el punto c
            if((mesh.faces[i].c == mesh.faces[j].a || mesh.faces[i].c == mesh.faces[j].b || mesh.faces[i].c == mesh.faces[j].c)) {
                index indice;
                indice.i = j;
                array_push(adyacentes_c, indice);
            }
        mesh.faces[i].c_adyacentes = adyacentes_c;
    }

    for(int i=0; i<n_faces; i++) {
        printf("\n------------------\nCara %d\n", i+1);
        printf("   a: \n");
        for(int j=0; j<array_length(mesh.faces[i].a_adyacentes); j++) {
            printf("   %d", (mesh.faces[i].a_adyacentes[j]).i+1);
        }
        printf("\n   b: \n");
        for(int j=0; j<array_length(mesh.faces[i].b_adyacentes); j++) {
            printf("   %d", (mesh.faces[i].b_adyacentes[j]).i+1);
        }
        printf("\n   c: \n");
        for(int j=0; j<array_length(mesh.faces[i].c_adyacentes); j++) {
            printf("   %d", (mesh.faces[i].c_adyacentes[j]).i+1);
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

vec3_t calcular_normal_vertice(index* caras_adyacentes) {
    vec3_t normal_sum = {0, 0, 0};
    int n = array_length(caras_adyacentes);

    // Sumatoria de las normales
    for (int i = 0; i < n; i++) {
        int face_index = caras_adyacentes[i].i;
        normal_sum = vec3_add(normal_sum, mesh.faces[face_index].normal);
    }

    // Promedio
    /*if (n > 0)
        normal_sum = vec3_div(normal_sum, n);*/
    
    normal_sum.x = - normal_sum.x;
    normal_sum.y = - normal_sum.y;
    normal_sum.z = - normal_sum.z;
    // Normalizar el vector
    vec3_normalize(&normal_sum);
    return normal_sum;
}

void update(void) {
    //cube_rotation.x += 0.02;
    /*
    cube_rotation.x += 0.01;
    cube_rotation.y += 0.02;
    cube_rotation.z += 0.04;
    */
    //cube_rotation.x = 60;
    //cube_rotation.y = 90;
    //cube_rotation.z = 52;

    cube_rotation.x = 4.9;
    cube_rotation.y = 5.89;
    cube_rotation.z = 3.69;

    cube_translation.z = 40.0;

    luz.direction = vec3_rotate_y(luz.direction, delta_angulo_luz);
    angulo_luz += delta_angulo_luz;
    if (angulo_luz > 2 * M_PI) {
        angulo_luz -= 2 * M_PI;
    }

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

        if(debug3) {
            printf("\033[93mCara %d\n\033[0m", i+1);
            printf("  \033[96ma: \033[0m%f %f %f\n", a_transformado.x, a_transformado.y, a_transformado.z);
            printf("  \033[96mb: \033[0m%f %f %f\n", b_transformado.x, b_transformado.y, b_transformado.z);
            printf("  \033[96mc: \033[0m%f %f %f\n", c_transformado.x, c_transformado.y, c_transformado.z);
            if(i==n_faces-1)
                debug3 = false;
        }
        // Gouraud Shading
        if(flag_shader) {

            // Para cada cara
            face_t* face = &(mesh.faces[i]);

            // Calcular normales por vértice usando las caras adyacentes
            face->normal_a = calcular_normal_vertice(face->a_adyacentes);
            face->normal_b = calcular_normal_vertice(face->b_adyacentes);
            face->normal_c = calcular_normal_vertice(face->c_adyacentes);

            if(debug2 && i==0) {
                printf("\033[92mVector 0\n");
                printf("normal a: %f %f %f\n", face->normal_a.x, face->normal_a.y, face->normal_a.z);
                printf("normal b: %f %f %f\n", face->normal_b.x, face->normal_b.y, face->normal_b.z);
                printf("normal c: %f %f %f\n", face->normal_c.x, face->normal_c.y, face->normal_c.z);
                debug2 = false;
            }

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

float phong_lighting(vec3_t normal) {
    vec3_t direccion_camara = {0, 0, 1};

    // Luz ambiente
    float ambiente = luz.ambient;

    // Luz difusa
    vec3_t luz_n = luz.direction;
    vec3_normalize(&luz_n);
    float difusa = fmax(vec3_dot(luz_n, normal), 0.0f);

    // Luz especular
    vec3_t reflect_dir = vec3_mul(normal, 2.0f * vec3_dot(normal, luz_n));
    vec3_normalize(&reflect_dir);
    reflect_dir = vec3_sub(reflect_dir, luz_n);
    vec3_normalize(&reflect_dir);
    
    vec3_t camara_n = direccion_camara;
    vec3_normalize(&camara_n);
    float brillo = 32.0f;
    float specular_coeff = 1.0f;
    float spec = pow(fmax(vec3_dot(reflect_dir, camara_n), 0.0f), brillo);
    float specular = specular_coeff * spec;

    // Iluminación
    float phong = ambiente + difusa + specular;
    if(phong < 0) phong = 0;
    if(phong > 1) phong = 1;
    return phong;
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
                if (flag_shader) {
                    /*
                    vec3_t normal = vec3_normal(a_transformado, b_transformado, c_transformado);
                    float I = - (vec3_dot(normal, luz.direction));
                    color = light_apply_intensity(WHITE, I);
                    if(debug && I < 0) {
                        printf("\033[96mrender\033[0m\n");
                        printf("normal: %f %f %f\n", normal.x, normal.y, normal.z);
                        printf("I: %f\n", I);
                        debug = false;
                    }*/

                    float I_a = (vec3_dot(face.normal_a, luz.direction));
                    float I_b = (vec3_dot(face.normal_b, luz.direction));
                    float I_c = (vec3_dot(face.normal_c, luz.direction));
                    /*I_a = I;
                    I_b = I;
                    I_c = I;
                    if(I_a < 0) I_a = 0;
                    if(I_a > 1) I_a = 1;
                    if(I_b < 0) I_b = 0;
                    if(I_b > 1) I_b = 1;
                    if(I_c < 0) I_c = 0;
                    if(I_c > 1) I_c = 1;*/
                    
                    I_a = phong_lighting(face.normal_a);
                    I_b = phong_lighting(face.normal_b);
                    I_c = phong_lighting(face.normal_c);

                    if(debug && i==0) {
                        printf("\033[96mrender cara %d\033[0m\n",i);
                        printf("normal a: %f %f %f\n", face.normal_a.x, face.normal_a.y, face.normal_a.z);
                        printf("normal b: %f %f %f\n", face.normal_b.x, face.normal_b.y, face.normal_b.z);
                        printf("normal c: %f %f %f\n", face.normal_c.x, face.normal_c.y, face.normal_c.z);
                        printf("I_a: %f\n", I_a);
                        printf("I_b: %f\n", I_b);
                        printf("I_c: %f\n", I_c);
                        
                    }
                    if(i==n_faces-1) {
                        debug = false;
                    }

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
                        WHITE, // Color base del triángulo
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