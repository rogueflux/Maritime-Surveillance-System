#include "sd_card_loader.h"

static FATFS fs;
static FIL file;
static bool sd_initialized = false;

bool SD_Card_Init(void) {
    if (FATFS_LinkDriver(&SD_Driver, SDPath) != 0) {
        return false;
    }

    if (f_mount(&fs, SDPath, 1) != FR_OK) {
        return false;
    }

    sd_initialized = true;
    return true;
}

bool SD_Card_LoadModel(uint8_t* model_buffer, uint32_t* model_size) {
    if (!sd_initialized) {
        return false;
    }

    if (f_open(&file, MODEL_FILENAME, FA_READ) != FR_OK) {
        return false;
    }

    UINT bytes_read;
    if (f_read(&file, model_buffer, MODEL_BUFFER_SIZE, &bytes_read) != FR_OK) {
        f_close(&file);
        return false;
    }

    *model_size = bytes_read;
    f_close(&file);
    return true;
}

bool SD_Card_IsAvailable(void) {
    return sd_initialized;
}
