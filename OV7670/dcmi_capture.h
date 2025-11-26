#ifndef DCMI_CAPTURE_H
#define DCMI_CAPTURE_H

#include "main.h"
#include "dma.h"
#include "dcmi.h"

// Frame buffer
extern uint8_t frame_buffer[IMAGE_BUFFER_SIZE];
extern volatile uint8_t frame_ready;

// Function prototypes
void DCMI_StartCapture(void);
void DCMI_StopCapture(void);
void DCMI_SuspendCapture(void);
void DCMI_ResumeCapture(void);
uint8_t Is_Frame_Ready(void);
void Reset_Frame_Ready(void);

// Callbacks
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi);
void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi);

#endif
