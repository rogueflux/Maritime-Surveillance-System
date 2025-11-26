# STM32H750 OV7670 Camera Integration

## Overview
This module implements a complete camera acquisition system using the OV7670 image sensor with STM32H750 microcontroller. The system captures real-time frames, preprocesses them, and prepares data for AI inference in submarine detection applications.

## Architecture

### Hardware Interface
- **I2C Configuration**: Camera register programming via I2C at address 0x42
- **DCMI (Digital Camera Interface)**: Parallel 8-bit data bus for pixel streaming
- **DMA Transfer**: Efficient frame buffer management without CPU intervention
- **GPIO Control**: VSYNC, HREF, PCLK synchronization signals

### Image Pipeline
OV7670 Sensor → DCMI Interface → RGB565 Buffer → RGB888 Conversion → Resize → Normalization → AI Model Input

## Key Features

### 1. Camera Configuration
- QQVGA (160x120) or QVGA (320x240) resolution support
- RGB565 color format output
- Automatic gain control and white balance
- Programmable via 100+ registers

### 2. Memory Management
- Frame buffer allocation in SDRAM
- Double-buffering capability
- DMA-driven data transfer
- Efficient memory layout for AI processing

### 3. Preprocessing Chain
- **RGB565 to RGB888 Conversion**: 16-bit to 24-bit color space
- **Image Resizing**: Bilinear interpolation to model input dimensions (320x320)
- **Pixel Normalization**: Float conversion with [0,1] scaling
- **Optimized Algorithms**: ARM Cortex-M7 optimized operations

## Technical Specifications

### Resolution Support
| Mode | Resolution | Buffer Size | Frame Rate |
|------|------------|-------------|------------|
| QQVGA | 160x120 | 38.4 KB | ~30 FPS |
| QVGA | 320x240 | 153.6 KB | ~15 FPS |

### Color Processing
- Input: RGB565 (16-bit)
- Intermediate: RGB888 (24-bit) 
- Output: Normalized float32 [0,1]

### Performance
- DMA-based capture: Zero CPU overhead during acquisition
- Preprocessing time: <50ms per frame
- Memory bandwidth optimized for continuous operation

## Integration Points

### AI Model Ready
Output format directly compatible with TensorFlow Lite:
- Float32 tensor
- NHWC layout (1, 320, 320, 3)
- Normalized pixel values

### System Monitoring
- Frame-ready interrupts
- Error handling callbacks
- Status LED indicators
- UART debug output

## Theory of Operation

The OV7670 sensor streams pixel data through the DCMI interface, synchronized by vertical (VSYNC) and horizontal (HREF) reference signals. Each pixel clock (PCLK) transfers one byte of data. The STM32's DMA controller assembles complete frames in SDRAM, triggering an interrupt when capture completes. The preprocessing pipeline then converts the native RGB565 format to the normalized floating-point representation required by neural network inference.

This hardware-accelerated approach ensures minimal CPU involvement, making it suitable for real-time submarine detection applications where both processing efficiency and power consumption are critical constraints.
