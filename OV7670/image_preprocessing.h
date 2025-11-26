#ifndef IMAGE_PREPROCESSING_H
#define IMAGE_PREPROCESSING_H

#include "main.h"

// Model input dimensions
#define MODEL_INPUT_WIDTH   320
#define MODEL_INPUT_HEIGHT  320
#define MODEL_CHANNELS      3

// Function prototypes
void RGB565_to_RGB888(uint8_t *src, uint8_t *dst, uint32_t width, uint32_t height);
void Resize_Image(uint8_t *src, uint8_t *dst, 
                  uint32_t src_width, uint32_t src_height,
                  uint32_t dst_width, uint32_t dst_height);
void Normalize_Image(uint8_t *src, float *dst, uint32_t size);
void Preprocess_Frame(uint8_t *frame_buffer, float *model_input);

#endif
