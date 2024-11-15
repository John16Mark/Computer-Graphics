#ifndef INC_3DRENDERER_VECTOR_H
#define INC_3DRENDERER_VECTOR_H

#include <math.h>

typedef struct {
    float x;
    float y;
} vec2_t;

// Vector 2D functions
float vec2_length(vec2_t v);
vec2_t vec2_add(vec2_t a, vec2_t b);
vec2_t vec2_sub(vec2_t a, vec2_t b);
vec2_t vec2_mul(vec2_t v, float factor);
vec2_t vec2_div(vec2_t v, float factor);
void vec2_normalize(vec2_t* v);

#endif //INC_3DRENDERER_VECTOR_H
