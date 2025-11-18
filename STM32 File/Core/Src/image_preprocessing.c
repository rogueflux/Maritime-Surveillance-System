#include "image_preprocessing.h"
#include <string.h>

// Simple JPEG placeholder - will use STM32 hardware JPEG later
bool Image_Preprocess_JPEGToRGB(const uint8_t* jpeg_data, uint32_t jpeg_size,
                               uint8_t* rgb_output, uint32_t output_size) {
    // Basic JPEG header check
    if (jpeg_size < 4 || jpeg_data[0] != 0xFF || jpeg_data[1] != 0xD8) {
        return false;
    }

    // Create a simple test pattern
    // In production, use STM32H750's hardware JPEG decoder
    for (uint32_t i = 0; i < MODEL_INPUT_WIDTH * MODEL_INPUT_HEIGHT * 3; i += 3) {
        rgb_output[i] = 100;     // R
        rgb_output[i + 1] = 150; // G
        rgb_output[i + 2] = 200; // B
    }

    return true;
}

// Keep your existing resize and normalize functions
void Image_ResizeRGB(const uint8_t* input_rgb, uint32_t input_width, uint32_t input_height,
                    uint8_t* output_rgb, uint32_t output_width, uint32_t output_height) {
    float x_ratio = (float)input_width / output_width;
    float y_ratio = (float)input_height / output_height;

    for (uint32_t y = 0; y < output_height; y++) {
        for (uint32_t x = 0; x < output_width; x++) {
            uint32_t input_x = (uint32_t)(x * x_ratio);
            uint32_t input_y = (uint32_t)(y * y_ratio);

            input_x = (input_x < input_width) ? input_x : input_width - 1;
            input_y = (input_y < input_height) ? input_y : input_height - 1;

            uint32_t input_idx = (input_y * input_width + input_x) * 3;
            uint32_t output_idx = (y * output_width + x) * 3;

            output_rgb[output_idx] = input_rgb[input_idx];
            output_rgb[output_idx + 1] = input_rgb[input_idx + 1];
            output_rgb[output_idx + 2] = input_rgb[input_idx + 2];
        }
    }
}

void Image_Normalize(const uint8_t* input_rgb, float* output_float,
                    uint32_t width, uint32_t height, uint32_t channels) {
    for (uint32_t i = 0; i < width * height * channels; i++) {
        output_float[i] = input_rgb[i] / 255.0f;
    }
}
