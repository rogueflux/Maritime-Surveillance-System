#ifndef YOLO_PROCESSOR_H
#define YOLO_PROCESSOR_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    float x;
    float y;
    float width;
    float height;
    float confidence;
    uint8_t class_id;
} BoundingBox;

typedef struct {
    BoundingBox* boxes;
    uint32_t count;
    uint32_t capacity;
} DetectionResults;

bool YOLO_ProcessOutput(const float* model_output, DetectionResults* results,
                       float confidence_threshold, float iou_threshold);
void YOLO_FreeResults(DetectionResults* results);
void YOLO_SendResultsUART(const DetectionResults* results);

#endif
