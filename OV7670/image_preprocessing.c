#include "image_preprocessing.h"
#include <math.h>

// Convert RGB565 to RGB888
void RGB565_to_RGB888(uint8_t *src, uint8_t *dst, uint32_t width, uint32_t height) {
    uint16_t *rgb565 = (uint16_t*)src;
    uint8_t *rgb888 = dst;
    
    for(uint32_t i = 0; i < width * height; i++) {
        uint16_t pixel = rgb565[i];
        
        // Extract RGB components from RGB565
        rgb888[i*3 + 0] = (pixel >> 11) & 0x1F;  // R
        rgb888[i*3 + 1] = (pixel >> 5)  & 0x3F;  // G  
        rgb888[i*3 + 2] = pixel & 0x1F;          // B
        
        // Scale to 8-bit
        rgb888[i*3 + 0] = (rgb888[i*3 + 0] * 255) / 31;
        rgb888[i*3 + 1] = (rgb888[i*3 + 1] * 255) / 63;
        rgb888[i*3 + 2] = (rgb888[i*3 + 2] * 255) / 31;
    }
}

// Simple bilinear resize
void Resize_Image(uint8_t *src, uint8_t *dst, 
                  uint32_t src_width, uint32_t src_height,
                  uint32_t dst_width, uint32_t dst_height) {
    
    float x_ratio = (float)src_width / dst_width;
    float y_ratio = (float)src_height / dst_height;
    
    for(uint32_t y = 0; y < dst_height; y++) {
        for(uint32_t x = 0; x < dst_width; x++) {
            uint32_t src_x = (uint32_t)(x * x_ratio);
            uint32_t src_y = (uint32_t)(y * y_ratio);
            
            // Simple nearest neighbor for performance
            uint32_t src_idx = (src_y * src_width + src_x) * 3;
            uint32_t dst_idx = (y * dst_width + x) * 3;
            
            dst[dst_idx + 0] = src[src_idx + 0];  // R
            dst[dst_idx + 1] = src[src_idx + 1];  // G
            dst[dst_idx + 2] = src[src_idx + 2];  // B
        }
    }
}

// Normalize pixel values to [0,1]
void Normalize_Image(uint8_t *src, float *dst, uint32_t size) {
    for(uint32_t i = 0; i < size; i++) {
        dst[i] = (float)src[i] / 255.0f;
    }
}

// Complete preprocessing pipeline
void Preprocess_Frame(uint8_t *frame_buffer, float *model_input) {
    uint8_t rgb888_buffer[OV7670_WIDTH * OV7670_HEIGHT * 3];
    uint8_t resized_buffer[MODEL_INPUT_WIDTH * MODEL_INPUT_HEIGHT * 3];
    
    // Step 1: Convert RGB565 to RGB888
    RGB565_to_RGB888(frame_buffer, rgb888_buffer, OV7670_WIDTH, OV7670_HEIGHT);
    
    // Step 2: Resize to model input size
    Resize_Image(rgb888_buffer, resized_buffer, 
                 OV7670_WIDTH, OV7670_HEIGHT,
                 MODEL_INPUT_WIDTH, MODEL_INPUT_HEIGHT);
    
    // Step 3: Normalize to [0,1]
    Normalize_Image(resized_buffer, model_input, MODEL_INPUT_WIDTH * MODEL_INPUT_HEIGHT * 3);
}
