#ifndef IMAGE_PREPROCESSING_H
#define IMAGE_PREPROCESSING_H

#include <stdbool.h>
#include <stdint.h>

// REDUCED RESOLUTION
#define MODEL_INPUT_WIDTH 160
#define MODEL_INPUT_HEIGHT 120
#define MODEL_INPUT_CHANNELS 3

bool Image_Preprocess_JPEGToRGB(const uint8_t* jpeg_data, uint32_t jpeg_size,
                               uint8_t* rgb_output, uint32_t output_size);
void Image_ResizeRGB(const uint8_t* input_rgb, uint32_t input_width, uint32_t input_height,
                    uint8_t* output_rgb, uint32_t output_width, uint32_t output_height);
void Image_Normalize(const uint8_t* input_rgb, float* output_float,
                    uint32_t width, uint32_t height, uint32_t channels);

#endif
