#include "vector.h"

// Vector 2D functions

float vec2_length(vec2_t v) {
    return sqrt(v.x*v.x + v.y*v.y);
}

vec2_t vec2_add(vec2_t a, vec2_t b) {
    vec2_t res;
    res.x = a.x+b.x;
    res.y = a.y+b.y;
    return res;
}

vec2_t vec2_sub(vec2_t a, vec2_t b) {
    vec2_t res;
    res.x = a.x-b.x;
    res.y = a.y-b.y;
    return res;
}

vec2_t vec2_mul(vec2_t v, float factor) {
    vec2_t res;
    res.x = v.x*factor;
    res.y = v.y*factor;
    return res;
}

vec2_t vec2_div(vec2_t v, float factor) {
    vec2_t res;
    res.x = v.x/factor;
    res.y = v.y/factor;
    return res;
}

void vec2_normalize(vec2_t* v) {
    float modulo = vec2_length(*v);
    v->x = v->x/modulo;
    v->y = v->y/modulo;
    return;
}

// Vector 3D functions

float vec3_length(vec3_t v) {
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

vec3_t vec3_add(vec3_t a, vec3_t b) {
    vec3_t res;
    res.x = a.x+b.x;
    res.y = a.y+b.y;
    res.z = a.z+b.z;
    return res;
}

vec3_t vec3_sub(vec3_t a, vec3_t b) {
    vec3_t res;
    res.x = a.x-b.x;
    res.y = a.y-b.y;
    res.z = a.z-b.z;
    return res;
}

vec3_t vec3_mul(vec3_t v, float factor) {
    vec3_t res;
    res.x = v.x*factor;
    res.y = v.y*factor;
    res.z = v.z*factor;
    return res;
}

vec3_t vec3_div(vec3_t v, float factor) {
    vec3_t res;
    res.x = v.x/factor;
    res.y = v.y/factor;
    res.z = v.z/factor;
    return res;
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
    vec3_t res;
    res.x = a.y*b.z - a.z*b.y;
    res.y = a.z*b.x - a.x*b.z;
    res.z = a.x*b.y - a.y*b.x;
    return res;
}

float vec3_dot(vec3_t a, vec3_t b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

void vec3_normalize(vec3_t* v) {
    float modulo = vec3_length(*v);
    v->x = v->x/modulo;
    v->y = v->y/modulo;
    v->z = v->z/modulo;
    return;
}



vec3_t vec3_rotate_x(vec3_t v, float angle) {
    vec3_t res;
    float seno = sin(angle);
    float coseno = cos(angle);
    res.x = v.x;
    res.y = v.y*coseno - v.z*seno;
    res.z = v.y*seno + v.z*coseno;
    return res;
}

vec3_t vec3_rotate_y(vec3_t v, float angle) {
    vec3_t res;
    float seno = sin(angle);
    float coseno = cos(angle);
    res.x = v.x*coseno + v.z*seno;
    res.y = v.y;
    res.z = v.z*coseno - v.x*seno;
    return res;
}

vec3_t vec3_rotate_z(vec3_t v, float angle) {
    vec3_t res;
    float seno = sin(angle);
    float coseno = cos(angle);
    res.x = v.x*coseno - v.y*seno;
    res.y = v.x*seno + v.y*coseno;
    res.z = v.z;
    return res;
}

// Vector conversion functions

vec4_t vec4_from_vec3(vec3_t v) {
    vec4_t res;
    res.x = v.x;
    res.y = v.y;
    res.z = v.z;
    res.w = 1.0;
    return res;
}

vec3_t vec3_from_vec4(vec4_t v) {
    vec3_t res;
    res.x = v.x;
    res.y = v.y;
    res.z = v.z;
    return res;
}