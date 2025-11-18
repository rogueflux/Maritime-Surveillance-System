📦 Overview
Total Images: 494 open-source submarine images

Classes: Submarine detection (single class)

Annotation Format: YOLOv8 supported labels

API & Model Hosting:

Explore, annotate, and deploy via Roboflow Universe

🚀 Getting Started
Download the dataset using the Roboflow Python SDK:

python
from roboflow import Roboflow
rf = Roboflow(api_key="YOUR_API_KEY")
project = rf.workspace("submarinedataset").project("submarine-detection-1-vapur")
version = project.version(1)
dataset = version.download("yolov8")
Replace "YOUR_API_KEY" with your Roboflow API key.

🧑‍🔬 Augmentation Techniques
Augmenting the dataset was critical for robustness. The following techniques—applied via Roboflow and customizable per version—help generalize in diverse maritime settings:

Brightness Adjustment
Simulates varying underwater lighting conditions

Contrast Variation
Helps the model generalize over murky vs. clear images

Rotation
Ensures detection regardless of submarine orientation

Scaling/Resizing
Trains the model on variable target sizes

Flipping (Horizontal/Vertical)
Improves detection if the submarine is flipped or mirrored

Noise Injection
Prepares the model for real-world sensor artifacts

Cropping & Random Erase
Encourages learning of partial or obscured submarine features

Note: Augmentations were completed prior to Version 1 generation. If re-training with YOLOv8, review all applied augmentations to avoid redundancy.

📊 Benchmark & Deployment
Model Training:
Trained and tested with YOLOv8 for submarine detection in underwater imagery.

Performance Metrics:
Track mAP, precision, recall, and more via the Roboflow dashboard.

Sample Images:
Mix of real and simulated scenes for comprehensive generalization.

📚 Citation
If you use this dataset in research or commercial applications, please cite:

text
@misc{submarine-detection-1-vapur_dataset,
  title = { Submarine Detection 1 Dataset },
  type = { Open Source Dataset },
  author = { SubmarineDataset },
  howpublished = { \url{https://universe.roboflow.com/submarinedataset/submarine-detection-1-vapur} },
  year = {2025},
  note = { visited on 2025-11-18 }
}
🛠 Resources
Roboflow Documentation

YOLOv8 Object Detection Guide

For questions, bug reports, or suggestions, please open an issue or reach out via Roboflow Universe.

