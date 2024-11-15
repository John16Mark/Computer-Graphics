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