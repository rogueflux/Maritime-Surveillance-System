#ifndef SD_CARD_LOADER_H
#define SD_CARD_LOADER_H

#include "fatfs.h"
#include <stdbool.h>
#include <stdint.h>

#define MODEL_FILENAME "model.tflite"
#define MODEL_BUFFER_SIZE (300 * 1024) // 300KB for model

bool SD_Card_Init(void);
bool SD_Card_LoadModel(uint8_t* model_buffer, uint32_t* model_size);
bool SD_Card_IsAvailable(void);

#endif
