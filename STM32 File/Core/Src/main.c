/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "app_x-cube-ai.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "uart_receiver.h"
#include "sd_card_loader.h"
#include "tflite_wrapper.h"
#include "image_preprocessing.h"
#include "yolo_processor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define JPEG_BUFFER_SIZE (120 * 120 * 2)      // Further reduced resolution
#define RGB_BUFFER_SIZE (120 * 120 * 3)       // Further reduced resolution
#define TENSOR_BUFFER_SIZE (120 * 120 * 3)    // Further reduced resolution
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

RTC_HandleTypeDef hrtc;

SD_HandleTypeDef hsd1;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart5_tx;

/* USER CODE BEGIN PV */
// Global contexts and buffers
TFLite_Context tflite_ctx;
DetectionResults detection_results;

uint8_t model_buffer[MODEL_BUFFER_SIZE];
uint8_t jpeg_frame_buffer[UART_RX_BUFFER_SIZE];
uint8_t rgb_image_buffer[RGB_BUFFER_SIZE];
float input_tensor_buffer[TENSOR_BUFFER_SIZE];

volatile uint32_t frame_counter = 0;
volatile uint32_t detection_counter = 0;
volatile uint32_t error_counter = 0;

// System status flags
volatile uint8_t system_initialized = 0;
volatile uint8_t model_loaded = 0;
volatile uint8_t inference_running = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_UART4_Init(void);
static void MX_UART5_Init(void);
static void MX_RTC_Init(void);
static void MX_SDMMC1_SD_Init(void);
/* USER CODE BEGIN PFP */
void ProcessCameraFrame(uint8_t* jpeg_data, uint16_t length);
void System_Status_Update(uint8_t status);
void Send_System_Info(void);
void LED_Error_Blink(uint8_t pattern);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Override HAL_GetTick for timing if needed
// uint32_t HAL_GetTick(void) {
//     return DWT->CYCCNT / (SystemCoreClock / 1000);
// }

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  uint32_t model_size = 0;
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  MX_RTC_Init();
  MX_SDMMC1_SD_Init();
  MX_FATFS_Init();
  MX_USB_DEVICE_Init();
  MX_X_CUBE_AI_Init();
  /* USER CODE BEGIN 2 */

    // Send startup message
    char startup_msg[] = "\r\n=== STM32H750 Submarine Detection System ===\r\n";
    HAL_UART_Transmit(&huart5, (uint8_t*)startup_msg, strlen(startup_msg), 1000);
    HAL_Delay(100);

    // Initialize system modules
    System_Status_Update(0); // LED off - initializing
    HAL_Delay(500);

    // Initialize UART receiver for camera data
    UART_Receiver_Init();
    char uart_msg[] = "UART Receiver initialized\r\n";
    HAL_UART_Transmit(&huart5, (uint8_t*)uart_msg, strlen(uart_msg), 1000);
    HAL_Delay(100);

    // Initialize SD Card
    HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_SET); // LED on during SD init
    char sd_init_msg[] = "Initializing SD Card...\r\n";
    HAL_UART_Transmit(&huart5, (uint8_t*)sd_init_msg, strlen(sd_init_msg), 1000);

    if (!SD_Card_Init()) {
      char error_msg[] = "ERROR: SD Card initialization failed - Check SD card connection\r\n";
      HAL_UART_Transmit(&huart5, (uint8_t*)error_msg, strlen(error_msg), 1000);
      LED_Error_Blink(1); // 1 blink pattern for SD card error
      Error_Handler();
    }

    char sd_msg[] = "SD Card initialized successfully\r\n";
    HAL_UART_Transmit(&huart5, (uint8_t*)sd_msg, strlen(sd_msg), 1000);
    HAL_Delay(100);

    // Load TFLite model from SD card
    char model_load_msg[] = "Loading model from SD card...\r\n";
    HAL_UART_Transmit(&huart5, (uint8_t*)model_load_msg, strlen(model_load_msg), 1000);

    if (!SD_Card_LoadModel(model_buffer, &model_size)) {
      char error_msg[150];
      snprintf(error_msg, sizeof(error_msg),
               "ERROR: Failed to load model from SD card\r\n"
               " - Ensure 'model.tflite' exists in root directory\r\n"
               " - File might be corrupted or too large\r\n");
      HAL_UART_Transmit(&huart5, (uint8_t*)error_msg, strlen(error_msg), 1000);
      LED_Error_Blink(2); // 2 blinks for model load error
      Error_Handler();
    }

    char model_msg[120];
    snprintf(model_msg, sizeof(model_msg),
             "Model loaded successfully: %lu bytes (%.2f KB)\r\n",
             model_size, (float)model_size / 1024.0f);
    HAL_UART_Transmit(&huart5, (uint8_t*)model_msg, strlen(model_msg), 1000);
    HAL_Delay(100);

    // Initialize TFLite interpreter
    char tflite_init_msg[] = "Initializing TFLite interpreter...\r\n";
    HAL_UART_Transmit(&huart5, (uint8_t*)tflite_init_msg, strlen(tflite_init_msg), 1000);

    if (!TFLite_Init(&tflite_ctx, model_buffer, model_size)) {
      char error_msg[] = "ERROR: TFLite initialization failed - Memory allocation error\r\n";
      HAL_UART_Transmit(&huart5, (uint8_t*)error_msg, strlen(error_msg), 1000);
      LED_Error_Blink(3); // 3 blinks for TFLite init error
      Error_Handler();
    }

    char tflite_msg[100];
    snprintf(tflite_msg, sizeof(tflite_msg),
             "TFLite interpreter initialized - Input: %lu, Output: %lu\r\n",  // Use %lu for uint32_t
             TFLite_GetInputSize(&tflite_ctx), TFLite_GetOutputSize(&tflite_ctx));
    HAL_UART_Transmit(&huart5, (uint8_t*)tflite_msg, strlen(tflite_msg), 1000);
    HAL_Delay(100);

    // Initialize detection results buffer
    char detection_init_msg[] = "Initializing detection system...\r\n";
    HAL_UART_Transmit(&huart5, (uint8_t*)detection_init_msg, strlen(detection_init_msg), 1000);

    detection_results.capacity = 10;
    detection_results.boxes = malloc(sizeof(BoundingBox) * detection_results.capacity);
    detection_results.count = 0;

    if (!detection_results.boxes) {
      char error_msg[] = "ERROR: Memory allocation failed for detection results\r\n";
      HAL_UART_Transmit(&huart5, (uint8_t*)error_msg, strlen(error_msg), 1000);
      LED_Error_Blink(4); // 4 blinks for memory allocation error
      Error_Handler();
    }

    // System initialized successfully
    system_initialized = 1;
    model_loaded = 1;

    // Send system ready message with summary
    char ready_msg[250];  // Increased buffer size
    snprintf(ready_msg, sizeof(ready_msg),
             "\r\n"
             "=== SYSTEM READY ===\r\n"
             "SD Card: OK\r\n"
             "Model: %lu bytes\r\n"
             "TFLite: Input %lu, Output %lu\r\n"  // Use %lu
             "Detection: %lu boxes\r\n"           // Use %lu
             "UART4: ESP32-CAM\r\n"
             "UART5: Output\r\n"
             "Status: Ready\r\n"
             "\r\n",
             model_size,
             TFLite_GetInputSize(&tflite_ctx),
             TFLite_GetOutputSize(&tflite_ctx),
             detection_results.capacity);
    HAL_UART_Transmit(&huart5, (uint8_t*)ready_msg, strlen(ready_msg), 1000);

    // Turn on status LED to indicate system ready
    System_Status_Update(1); // Solid LED - system ready

    // Quick LED test pattern
    for (int i = 0; i < 3; i++) {
        HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_RESET);
        HAL_Delay(100);
        HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_SET);
        HAL_Delay(100);
    }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // Process incoming UART frames from ESP32-CAM
    UART_Receiver_Process();

    uint16_t frame_length = 0;
    if (UART_Receiver_GetFrame(jpeg_frame_buffer, &frame_length)) {
      frame_counter++;

      // Process the received camera frame
      ProcessCameraFrame(jpeg_frame_buffer, frame_length);

      // Clear the frame buffer for next reception
      UART_Receiver_ClearFrame();

      // Send periodic system info every 10 frames
      if (frame_counter % 10 == 0) {
        Send_System_Info();
      }
    }

    // Add small delay to prevent overwhelming the system
    HAL_Delay(5);

    /* USER CODE END WHILE */

  MX_X_CUBE_AI_Process();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 480;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 20;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */
  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */
  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC1_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */
  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */
  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 0;
  if (HAL_SD_Init(&hsd1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SDMMC1_Init 2 */
  /* USER CODE END SDMMC1_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */
  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */
  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */
  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */
  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */
  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart5, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart5, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */
  /* USER CODE END UART5_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_STATUS_Pin */
  GPIO_InitStruct.Pin = LED_STATUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_STATUS_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/**
 * @brief Update system status LED
 * @param status: 0=Off, 1=Ready, 2=Processing, 3=Detection
 */
void System_Status_Update(uint8_t status) {
    static uint32_t last_blink_time = 0;

    switch(status) {
        case 0: // Off - Initializing
            HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_RESET);
            break;

        case 1: // Ready - Solid ON
            HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_SET);
            break;

        case 2: // Processing - Slow blink
            if (HAL_GetTick() - last_blink_time > 500) {
                HAL_GPIO_TogglePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin);
                last_blink_time = HAL_GetTick();
            }
            break;

        case 3: // Detection - Fast blink
            for (int i = 0; i < 3; i++) {
                HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_SET);
                HAL_Delay(100);
                HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_RESET);
                HAL_Delay(100);
            }
            HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_SET);
            break;
    }
}

/**
 * @brief Error LED blink pattern
 * @param pattern: Error pattern code
 */
void LED_Error_Blink(uint8_t pattern) {
    while(1) {
        for (int i = 0; i < pattern; i++) {
            HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_SET);
            HAL_Delay(300);
            HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_RESET);
            HAL_Delay(300);
        }
        HAL_Delay(1000);
    }
}

/**
 * @brief Send system information over UART
 */
void Send_System_Info(void) {
    char info_msg[100];
    snprintf(info_msg, sizeof(info_msg),
             "System Info - Frames: %lu, Detections: %lu, Errors: %lu\r\n",
             frame_counter, detection_counter, error_counter);
    HAL_UART_Transmit(&huart5, (uint8_t*)info_msg, strlen(info_msg), 1000);
}

/**
 * @brief Process camera frame and run inference
 */
void ProcessCameraFrame(uint8_t* jpeg_data, uint16_t length) {
    // Indicate inference is running
    inference_running = 1;
    System_Status_Update(2); // LED blink - processing

    // Convert JPEG to RGB
    if (!Image_Preprocess_JPEGToRGB(jpeg_data, length, rgb_image_buffer, RGB_BUFFER_SIZE)) {
        char error_msg[] = "ERROR: JPEG to RGB conversion failed\r\n";
        HAL_UART_Transmit(&huart5, (uint8_t*)error_msg, strlen(error_msg), 100);
        error_counter++;
        inference_running = 0;
        System_Status_Update(1); // Back to ready state
        return;
    }

    // Normalize image for model input
    Image_Normalize(rgb_image_buffer, input_tensor_buffer,
                    MODEL_INPUT_WIDTH, MODEL_INPUT_HEIGHT, MODEL_INPUT_CHANNELS);

    // Run inference
    if (TFLite_RunInference(&tflite_ctx, input_tensor_buffer)) {
        // Get output data
        const float* output_data = TFLite_GetOutputBuffer(&tflite_ctx);

        if (YOLO_ProcessOutput(output_data, &detection_results, 0.5f, 0.4f)) {
            detection_counter += detection_results.count;

            // Send detection results over UART5
            YOLO_SendResultsUART(&detection_results);

            // Fast LED blink pattern for detection
            System_Status_Update(3);

            // Add timestamp to detection if RTC is available
            char timestamp_msg[50];
            RTC_TimeTypeDef sTime;
            RTC_DateTypeDef sDate;

            HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
            HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

            snprintf(timestamp_msg, sizeof(timestamp_msg),
                     "Time: %02d:%02d:%02d Detections: %lu\r\n",
                     sTime.Hours, sTime.Minutes, sTime.Seconds, detection_results.count);
            HAL_UART_Transmit(&huart5, (uint8_t*)timestamp_msg, strlen(timestamp_msg), 100);

        } else {
            // No detections found
            char no_detect_msg[] = "No objects detected\r\n";
            HAL_UART_Transmit(&huart5, (uint8_t*)no_detect_msg, strlen(no_detect_msg), 100);
        }
    } else {
        char error_msg[] = "ERROR: Inference failed\r\n";
        HAL_UART_Transmit(&huart5, (uint8_t*)error_msg, strlen(error_msg), 100);
        error_counter++;
    }

    inference_running = 0;
    System_Status_Update(1); // Back to ready state
}

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();

  // Turn on LED to indicate error
  HAL_GPIO_WritePin(LED_STATUS_GPIO_Port, LED_STATUS_Pin, GPIO_PIN_SET);

  while (1)
  {
    // Optional: Send error message
    char error_msg[] = "Fatal error - system halted\r\n";
    HAL_UART_Transmit(&huart5, (uint8_t*)error_msg, strlen(error_msg), 1000);
    HAL_Delay(1000);
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  char assert_msg[100];
  snprintf(assert_msg, sizeof(assert_msg), "Assert failed: %s line %lu\r\n", file, line);
  HAL_UART_Transmit(&huart5, (uint8_t*)assert_msg, strlen(assert_msg), 1000);
  while (1) {}
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
