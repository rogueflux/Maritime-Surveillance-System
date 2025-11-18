#ifndef TFLITE_WRAPPER_H
#define TFLITE_WRAPPER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// Simple model context - no TensorFlow dependencies
typedef struct {
    uint8_t* model_data;
    size_t model_size;
    float* input_buffer;
    float* output_buffer;
    uint32_t input_size;
    uint32_t output_size;
} TFLite_Context;

bool TFLite_Init(TFLite_Context* ctx, uint8_t* model_data, size_t model_size);
bool TFLite_RunInference(TFLite_Context* ctx, const float* input_data);
void TFLite_Cleanup(TFLite_Context* ctx);
float* TFLite_GetOutputBuffer(TFLite_Context* ctx);
uint32_t TFLite_GetOutputSize(TFLite_Context* ctx);
uint32_t TFLite_GetInputSize(TFLite_Context* ctx);  // ADD THIS LINE

#endif
