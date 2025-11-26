#ifndef UART_COMMUNICATION_H
#define UART_COMMUNICATION_H

#include "main.h"
#include "usart.h"

// Function prototypes
void UART_Send_String(char *str);
void UART_Send_Detection_Result(uint8_t class_id, float confidence, 
                               uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void UART_Send_Image_Stats(uint32_t frame_count);

#endif
