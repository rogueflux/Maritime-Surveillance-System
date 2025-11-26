#include "dcmi_capture.h"
#include "ov7670_config.h"

uint8_t frame_buffer[IMAGE_BUFFER_SIZE] __attribute__((section(".sdram")));
volatile uint8_t frame_ready = 0;

void DCMI_StartCapture(void) {
    frame_ready = 0;
    HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)frame_buffer, IMAGE_BUFFER_SIZE/4);
}

void DCMI_StopCapture(void) {
    HAL_DCMI_Stop(&hdcmi);
}

void DCMI_SuspendCapture(void) {
    HAL_DCMI_Suspend(&hdcmi);
}

void DCMI_ResumeCapture(void) {
    HAL_DCMI_Resume(&hdcmi);
}

uint8_t Is_Frame_Ready(void) {
    return frame_ready;
}

void Reset_Frame_Ready(void) {
    frame_ready = 0;
}

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi) {
    frame_ready = 1;
}

void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi) {
    // Handle DCMI error
    Error_Handler();
}
