#include "tflite_wrapper.h"
#include <string.h>

bool TFLite_Init(TFLite_Context* ctx, uint8_t* model_data, size_t model_size) {
    if (!ctx || !model_data || model_size == 0) {
        return false;
    }

    ctx->model_data = model_data;
    ctx->model_size = model_size;

    // REDUCED BUFFER SIZES
    ctx->input_size = 120 * 120 * 3;  // Reduced from 160x120 to 120x120
    ctx->output_size = 500;           // Reduced from 1000

    ctx->input_buffer = (float*)malloc(ctx->input_size * sizeof(float));
    ctx->output_buffer = (float*)malloc(ctx->output_size * sizeof(float));

    if (!ctx->input_buffer || !ctx->output_buffer) {
        TFLite_Cleanup(ctx);
        return false;
    }

    return true;
}

uint32_t TFLite_GetInputSize(TFLite_Context* ctx) {
    return ctx ? ctx->input_size : 0;
}


bool TFLite_RunInference(TFLite_Context* ctx, const float* input_data) {
    if (!ctx || !input_data) {
        return false;
    }

    // Copy input data
    memcpy(ctx->input_buffer, input_data, ctx->input_size * sizeof(float));

    // SIMPLE PLACEHOLDER INFERENCE
    // In production, this would run actual model inference
    // For now, create dummy detection outputs
    for (uint32_t i = 0; i < ctx->output_size; i++) {
        ctx->output_buffer[i] = 0.0f;
    }

    // Add a dummy detection for testing
    if (ctx->output_size >= 6) {
        // Format: [x, y, w, h, confidence, class]
        ctx->output_buffer[0] = 0.5f; // x
        ctx->output_buffer[1] = 0.5f; // y
        ctx->output_buffer[2] = 0.3f; // w
        ctx->output_buffer[3] = 0.3f; // h
        ctx->output_buffer[4] = 0.8f; // confidence
        ctx->output_buffer[5] = 0.0f; // class (submarine)
    }

    return true;
}

void TFLite_Cleanup(TFLite_Context* ctx) {
    if (ctx) {
        if (ctx->input_buffer) {
            free(ctx->input_buffer);
            ctx->input_buffer = NULL;
        }
        if (ctx->output_buffer) {
            free(ctx->output_buffer);
            ctx->output_buffer = NULL;
        }
    }
}

float* TFLite_GetOutputBuffer(TFLite_Context* ctx) {
    return ctx ? ctx->output_buffer : NULL;
}

uint32_t TFLite_GetOutputSize(TFLite_Context* ctx) {
    return ctx ? ctx->output_size : 0;
}
