#include "matrix.h"

mat4_t mat4_identity(void) {
    mat4_t res;
    int i, j;
    for(i=0; i<4; i++) {
        for(j=0; j<4; j++) {
            res.m[i][j] = i==j ? 1 : 0;
        }
    }
    return res;
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
    mat4_t res = mat4_identity();
    res.m[0][0] = sx;
    res.m[1][1] = sy;
    res.m[2][2] = sz;
    return res;
}

mat4_t mat4_make_translation(float tx, float ty, float tz) {
    mat4_t res = mat4_identity();
    res.m[0][3] = tx;
    res.m[1][3] = ty;
    res.m[2][3] = tz;
    return res;
}

mat4_t mat4_make_rotation_x(float angle) {
    mat4_t res = mat4_identity();
    float seno = sin(angle);
    float coseno = cos(angle);
    res.m[1][1] = coseno;
    res.m[1][2] = -seno;
    res.m[2][1] = seno;
    res.m[2][2] = coseno;
    return res;
}

mat4_t mat4_make_rotation_y(float angle) {
    mat4_t res = mat4_identity();
    float seno = sin(angle);
    float coseno = cos(angle);
    res.m[0][0] = coseno;
    res.m[0][2] = seno;
    res.m[2][0] = -seno;
    res.m[2][2] = coseno;
    return res;
}

mat4_t mat4_make_rotation_z(float angle) {
    mat4_t res = mat4_identity();
    float seno = sin(angle);
    float coseno = cos(angle);
    res.m[0][0] = coseno;
    res.m[0][1] = -seno;
    res.m[1][0] = seno;
    res.m[1][1] = coseno;
    return res;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
    vec4_t res;
    res.x = v.x*m.m[0][0] + v.y*m.m[0][1] + v.z*m.m[0][2] + v.w*m.m[0][3];
    res.y = v.x*m.m[1][0] + v.y*m.m[1][1] + v.z*m.m[1][2] + v.w*m.m[1][3];
    res.z = v.x*m.m[2][0] + v.y*m.m[2][1] + v.z*m.m[2][2] + v.w*m.m[2][3];
    res.w = v.x*m.m[3][0] + v.y*m.m[3][1] + v.z*m.m[3][2] + v.w*m.m[3][3];
    return res;
}

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b) {
    mat4_t res;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            res.m[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                res.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return res;
}