#include "uart_communication.h"
#include <stdio.h>

void UART_Send_String(char *str) {
    HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), 100);
}

void UART_Send_Detection_Result(uint8_t class_id, float confidence, 
                               uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    char buffer[128];
    snprintf(buffer, sizeof(buffer), 
             "DETECT: class=%d, conf=%.2f, x=%d, y=%d, w=%d, h=%d\r\n",
             class_id, confidence, x, y, width, height);
    UART_Send_String(buffer);
}

void UART_Send_Image_Stats(uint32_t frame_count) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "FRAME: %lu\r\n", frame_count);
    UART_Send_String(buffer);
}
