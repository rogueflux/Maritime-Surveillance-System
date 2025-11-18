#ifndef UART_RECEIVER_H
#define UART_RECEIVER_H

#include <stdbool.h>
#include <stdint.h>

#define UART_RX_BUFFER_SIZE 4096  // Reduced from 8192 to save memory
#define FRAME_HEADER 0xAABB
#define FRAME_FOOTER 0xCCDD

typedef struct {
    uint8_t data[UART_RX_BUFFER_SIZE];
    uint16_t length;
    bool frame_ready;
} FrameBuffer;

void UART_Receiver_Init(void);
void UART_Receiver_Process(void);
bool UART_Receiver_GetFrame(uint8_t* buffer, uint16_t* length);
void UART_Receiver_ClearFrame(void);

#endif
