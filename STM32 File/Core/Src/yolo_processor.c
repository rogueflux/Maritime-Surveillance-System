#include "yolo_processor.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

extern UART_HandleTypeDef huart5;

static float CalculateIOU(const BoundingBox* box1, const BoundingBox* box2) {
    float x1 = fmaxf(box1->x - box1->width / 2, box2->x - box2->width / 2);
    float y1 = fmaxf(box1->y - box1->height / 2, box2->y - box2->height / 2);
    float x2 = fminf(box1->x + box1->width / 2, box2->x + box2->width / 2);
    float y2 = fminf(box1->y + box1->height / 2, box2->y + box2->height / 2);

    float intersection = fmaxf(0, x2 - x1) * fmaxf(0, y2 - y1);
    float area1 = box1->width * box1->height;
    float area2 = box2->width * box2->height;

    return intersection / (area1 + area2 - intersection);
}

bool YOLO_ProcessOutput(const float* model_output, DetectionResults* results,
                       float confidence_threshold, float iou_threshold) {
    // This is a simplified YOLO output parser
    // Adjust based on your specific YOLO model output format

    uint32_t num_boxes = 0;
    const uint32_t max_boxes = 100;

    // Temporary array for candidate detections
    BoundingBox candidates[max_boxes];

    // Parse output tensor (adjust indices based on your model)
    for (uint32_t i = 0; i < max_boxes; i++) {
        const float* box_data = &model_output[i * 6]; // x, y, w, h, conf, class

        float confidence = box_data[4];
        if (confidence < confidence_threshold) {
            continue;
        }

        candidates[num_boxes].x = box_data[0];
        candidates[num_boxes].y = box_data[1];
        candidates[num_boxes].width = box_data[2];
        candidates[num_boxes].height = box_data[3];
        candidates[num_boxes].confidence = confidence;
        candidates[num_boxes].class_id = (uint8_t)box_data[5];

        num_boxes++;
        if (num_boxes >= max_boxes) break;
    }

    // Non-maximum suppression
    for (uint32_t i = 0; i < num_boxes; i++) {
        for (uint32_t j = i + 1; j < num_boxes; j++) {
            if (candidates[i].class_id == candidates[j].class_id) {
                float iou = CalculateIOU(&candidates[i], &candidates[j]);
                if (iou > iou_threshold) {
                    if (candidates[i].confidence > candidates[j].confidence) {
                        candidates[j].confidence = 0;
                    } else {
                        candidates[i].confidence = 0;
                    }
                }
            }
        }
    }

    // Add valid detections to results
    results->count = 0;
    for (uint32_t i = 0; i < num_boxes; i++) {
        if (candidates[i].confidence >= confidence_threshold) {
            if (results->count < results->capacity) {
                results->boxes[results->count] = candidates[i];
                results->count++;
            }
        }
    }

    return (results->count > 0);
}

void YOLO_FreeResults(DetectionResults* results) {
    if (results->boxes) {
        free(results->boxes);
        results->boxes = NULL;
        results->count = 0;
        results->capacity = 0;
    }
}

void YOLO_SendResultsUART(const DetectionResults* results) {
    char buffer[128];

    sprintf(buffer, "Detections: %lu\r\n", results->count);
    HAL_UART_Transmit(&huart5, (uint8_t*)buffer, strlen(buffer), 1000);

    for (uint32_t i = 0; i < results->count; i++) {
        const BoundingBox* box = &results->boxes[i];
        sprintf(buffer, "Box %lu: class=%u, conf=%.2f, x=%.2f, y=%.2f, w=%.2f, h=%.2f\r\n",
                i, box->class_id, box->confidence, box->x, box->y, box->width, box->height);
        HAL_UART_Transmit(&huart5, (uint8_t*)buffer, strlen(buffer), 1000);
    }
}
