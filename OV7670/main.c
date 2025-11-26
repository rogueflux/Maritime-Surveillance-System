#include "main.h"
#include "i2c.h"
#include "dcmi.h"
#include "dma.h"
#include "sdmmc.h"
#include "fatfs.h"
#include "usart.h"
#include "gpio.h"
#include "ov7670_config.h"
#include "dcmi_capture.h"
#include "image_preprocessing.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"

// TensorFlow Lite setup
#define TENSOR_ARENA_SIZE 200000
static uint8_t tensor_arena[TENSOR_ARENA_SIZE] __attribute__((section(".sdram")));

// Model input buffer
static float model_input[MODEL_INPUT_WIDTH * MODEL_INPUT_HEIGHT * MODEL_CHANNELS];

// Function prototypes
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DCMI_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SDMMC1_SD_Init(void);
static void MX_FATFS_Init(void);
void Error_Handler(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    
    // Initialize all peripherals
    MX_GPIO_Init();
    MX_DCMI_Init();
    MX_I2C1_Init();
    MX_USART2_UART_Init();
    MX_SDMMC1_SD_Init();
    MX_FATFS_Init();
    
    // Initialize camera
    if(OV7670_Init() != HAL_OK) {
        Error_Handler();
    }
    
    // Initialize TensorFlow Lite model (to be implemented)
    // if(Load_Model_From_SD() != HAL_OK) {
    //     Error_Handler();
    // }
    
    // Status LED indication
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
    
    printf("Submarine Detection System Started\r\n");
    printf("OV7670 Camera Initialized\r\n");
    
    // Start continuous capture
    DCMI_StartCapture();
    
    while(1) {
        // Wait for a new frame
        if(Is_Frame_Ready()) {
            // Turn on LED during processing
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
            
            // Preprocess the frame
            Preprocess_Frame(frame_buffer, model_input);
            
            // Run inference (to be implemented)
            // Run_Inference(model_input);
            
            // Send results via UART (to be implemented)
            // Send_Detection_Results();
            
            // Reset frame flag
            Reset_Frame_Ready();
            
            // Turn off LED after processing
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
        }
        
        // Add small delay to prevent overwhelming the system
        HAL_Delay(10);
    }
}

void Error_Handler(void) {
    // Blink LED rapidly on error
    while(1) {
        HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_3);
        HAL_Delay(100);
    }
}

// Override HAL_Delay for more precise timing if needed
void HAL_Delay(uint32_t Delay) {
    uint32_t tickstart = HAL_GetTick();
    uint32_t wait = Delay;
    
    while((HAL_GetTick() - tickstart) < wait) {
        __NOP();
    }
}
