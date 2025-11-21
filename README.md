# STM32-Based Rogue Submarine Detection System Using Embedded AI and Edge Computing

Autonomous, low-power underwater object detection—using state-of-the-art TinyML deep learning on affordable embedded hardware.

## Overview

This repository documents a real-time, embedded system for identifying underwater threats (submarines, mines, UUVs) in maritime environments, leveraging:

- STM32H750 microcontroller (ARM Cortex-M7, 480MHz)
- ESP32-CAM module for image acquisition
- YOLOv8 Nano model, quantized with INT8 and deployed via TensorFlow Lite Micro
- Roboflow-curated dataset, with rich underwater augmentations

## Table of Contents

- [Objectives](#-objectives)
- [System Architecture](#-system-architecture)
- [Dataset & Augmentation](#-dataset--augmentation)
- [Model Training & Deployment](#-model-training--deployment)
- [Implementation & Code Flow](#-implementation--code-flow)
- [Results & Benchmarks](#-results--benchmarks)
- [Future Work](#-future-work)
- [Project Team](#-project-team)
- [References](#-references)

## Objectives

- Embed AI-powered submarine detection on resource-constrained edge hardware
- Optimize for low-latency (<200ms), <1W power, and <$50 BOM costs
- Architect robust, field-updateable systems with SD card model loading
- Deliver validated detection accuracy (>87% TPR), with modular code and full reproducibility

## System Architecture

### Hardware

- **ESP32-CAM**: WiFi/UART image streaming; OV2640 sensor; <240mW power consumption
- **STM32H750VBT6**: ARM Cortex-M7, 1MB SRAM, hardware JPEG codec, SD card slot, <400mW
- **FTDI USB-to-Serial**: For host telemetry and monitoring

### Two-Stage Pipeline

- **Dev Phase**: WiFi MJPEG streaming from ESP32-CAM to Python/YOLO on laptop
- **Edge Phase**: UART DMA transfer from ESP32-CAM → STM32H750 → TFLite Micro inference

## Dataset & Augmentation

- **Sources**: Roboflow Universe, maritime surveillance, public and synthetic images
- **Classes**: Submarine (single-class)
- **Images**: ~1,200 originals, expanded x3 via augmentations
- **Splits**: 70% train, 20% val, 10% test

### Augmentation Pipeline

- **Geometric**: Horizontal (50%), vertical (25%) flip, rotation (±15°), crop (0-20%)
- **Photometric**: Brightness (+/-25%), contrast, exposure, saturation
- **Degradation**: Salt-and-pepper noise, Gaussian blur, JPEG artifacts
- **Advanced**: Mosaic, mixup, cutout

All exported in YOLOv8 format (normalized bounding box txt).

## Model Training & Deployment

- YOLOv8n (Nano) trained for 150 epochs on Google Colab with AdamW/cosine annealing
- INT8 quantization (FP32 → ONNX → TFLite int8), <2MB final model
- Deployed with STM32Cube.AI for 70% latency reduction, fit into <1MB SRAM
- SD card allows in-field model updates (model.tflite)

## Implementation & Code Flow

- ESP32-CAM streams frames via MJPEG (WiFi) or UART (921600 baud); each frame has a header/signature
- STM32 firmware (STM32CubeIDE):
  - UART DMA buffer for JPEG frame ingest
  - SD card FatFS for model loading
  - TJpgDec for JPEG decompression
  - Preprocessing, NN inference, NMS on detections
- USB Serial telemetry to host for real-time alerting

## Results & Benchmarks

- **Accuracy**: mAP@0.5 = 0.91 (FP32), mAP@0.5 = 0.89 (INT8)
- **Performance**: 2.3 FPS throughput, 80–150ms inference latency per frame (STM32H750)
- **Power**: <1W total system consumption; battery-friendly
- **Scalability**: Multi-node affordable deployment (<$50/unit)

## Future Work

- **Communication**: LoRa telemetry, SPI uplinks, mesh networking
- **Preprocessing**: Optimized color correction (CLAHE, channel boosting)
- **Advanced AI**: Instance segmentation, multi-class, 3D tracking, transformer-based models
- **Hardware**: Multi-camera fusion, FPGA migration, environmental adaptation
- **Logging & Security**: On-device event logging, encryption, federated learning for fleet improvement

## Project Team

- Priyanshu Sil (ECE, MIT Manipal)
- Steve Augustian (ECE, MIT Manipal)
- Mayank Parashar (ECE, MIT Manipal)
- Guide: Dr. Rahul Ratnakumar (Assistant Professor, ECE Dept.)

## References

See `Minor-Project-Report.pdf` for full technical background, diagrams, and citations.

- Roboflow Universe Datasets
- Ultralytics YOLOv8 docs
- STMicroelectronics & WeAct Studio datasheets
- TensorFlow Lite Micro & STM32Cube.AI toolkit
