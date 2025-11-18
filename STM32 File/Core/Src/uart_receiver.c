#include "uart_receiver.h"
#include <string.h>
#include "main.h"

extern UART_HandleTypeDef huart4;  // Use UART4 for ESP32-CAM

static FrameBuffer frame_buffer;
static uint8_t uart_rx_byte;
static enum {
    STATE_WAIT_HEADER,
    STATE_READ_LENGTH,
    STATE_READ_DATA,
    STATE_WAIT_FOOTER
} receiver_state = STATE_WAIT_HEADER;

static uint16_t expected_length = 0;
static uint16_t bytes_received = 0;
static uint8_t header_bytes[2] = {0};
static uint8_t header_index = 0;

void UART_Receiver_Init(void) {
    frame_buffer.frame_ready = false;
    frame_buffer.length = 0;
    HAL_UART_Receive_DMA(&huart4, &uart_rx_byte, 1);
}

void UART_Receiver_Process(void) {
    static uint8_t byte;

    if (HAL_UART_Receive(&huart4, &byte, 1, 0) == HAL_OK) {
        switch (receiver_state) {
            case STATE_WAIT_HEADER:
                header_bytes[header_index++] = byte;
                if (header_index == 2) {
                    if ((header_bytes[0] == 0xAA) && (header_bytes[1] == 0xBB)) {
                        receiver_state = STATE_READ_LENGTH;
                    } else {
                        header_index = 0;
                    }
                }
                break;

            case STATE_READ_LENGTH:
                if (header_index == 2) {
                    expected_length = header_bytes[0] << 8;
                    header_index++;
                } else if (header_index == 3) {
                    expected_length |= header_bytes[1];
                    bytes_received = 0;
                    receiver_state = STATE_READ_DATA;
                }
                break;

            case STATE_READ_DATA:
                if (bytes_received < UART_RX_BUFFER_SIZE) {
                    frame_buffer.data[bytes_received++] = byte;
                }
                if (bytes_received >= expected_length) {
                    receiver_state = STATE_WAIT_FOOTER;
                }
                break;

            case STATE_WAIT_FOOTER:
                if (byte == 0xCC) {
                    uint8_t next_byte;
                    if (HAL_UART_Receive(&huart4, &next_byte, 1, 10) == HAL_OK) {  // FIXED: huart1 to huart4
                        if (next_byte == 0xDD) {
                            frame_buffer.length = bytes_received;
                            frame_buffer.frame_ready = true;
                        }
                    }
                }
                receiver_state = STATE_WAIT_HEADER;
                header_index = 0;
                break;
        }
    }
}

bool UART_Receiver_GetFrame(uint8_t* buffer, uint16_t* length) {
    if (frame_buffer.frame_ready) {
        memcpy(buffer, frame_buffer.data, frame_buffer.length);
        *length = frame_buffer.length;
        frame_buffer.frame_ready = false;
        return true;
    }
    return false;
}

void UART_Receiver_ClearFrame(void) {
    frame_buffer.frame_ready = false;
}
