# Submarine Detection Dataset

## Overview
This repository contains an object detection dataset for identifying submarines in underwater imagery, tailored for use with YOLOv8 and other modern deep learning frameworks.

### Dataset Specifications
- **Total Images**: 494 open-source images of submarines
- **Classes**: Single class detection (Submarine)
- **Annotation Format**: YOLOv8 compatible labels
- **Platform**: Enhanced dataset browsing, annotation, and model deployment via Roboflow

## Augmentation Techniques
Augmenting the dataset is critical for creating robust object detection models, especially for underwater and maritime tasks where environmental factors vary significantly.

The following augmentations were applied via Roboflow (customizable per version):

- **Brightness Adjustment**: Simulates varying underwater lighting conditions
- **Contrast Variation**: Helps model generalize over murky vs. clear images
- **Rotation**: Ensures detection regardless of submarine orientation
- **Scaling/Resizing**: Trains the model on variable target sizes
- **Flipping (Horizontal/Vertical)**: Improves detection if the submarine is flipped or mirrored
- **Noise Injection**: Prepares the model for real-world sensor artifacts
- **Cropping & Random Erase**: Encourages learning of partial or obscured submarine features

> **Important Note**: All augmentation steps were completed prior to generating Version 1 of the dataset. If using YOLOv8 training, review augmentations applied through both Roboflow and your training pipeline to avoid redundancy.

## Benchmark & Deployment

### Model Training
- **Framework**: Tested with YOLOv8 for submarine detection
- **Environment**: Optimized for underwater imagery analysis

### Performance Metrics
Track the following metrics via Roboflow dashboard:
- mAP (mean Average Precision)
- Precision
- Recall
- Comprehensive analytics

### Dataset Composition
- Includes a mix of real and simulated submarine scenes
- Designed for comprehensive model generalization
- Suitable for various underwater detection scenarios
