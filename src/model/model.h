#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <stdint.h>

#define DEFAULT_CALIBRATION 2048

typedef struct {
    uint16_t    calibration;
    double      current;
    long double current_sum;
} model_t;

#endif