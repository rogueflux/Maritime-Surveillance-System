#include <math.h>

// Simple implementations of missing math functions
float fmaxf(float x, float y) {
    return (x > y) ? x : y;
}

float fminf(float x, float y) {
    return (x < y) ? x : y;
}

float fabsf(float x) {
    return (x < 0) ? -x : x;
}
