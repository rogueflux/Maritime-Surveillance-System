import serial
import re
import json
from datetime import datetime

class DetectionSerialReader:
    def __init__(self, port='COM3', baudrate=115200):
        self.ser = serial.Serial(port, baudrate, timeout=1)
        self.detection_pattern = re.compile(
            r'Box (\d+): class=(\d+), conf=([\d.]+), x=([\d.]+), y=([\d.]+), w=([\d.]+), h=([\d.]+)'
        )
    
    def read_detections(self):
        detections = []
        
        while self.ser.in_waiting > 0:
            line = self.ser.readline().decode('utf-8').strip()
            
            if line.startswith('Detections:'):
                print(f"\n[{datetime.now()}] {line}")
            
            match = self.detection_pattern.match(line)
            if match:
                detection = {
                    'box_id': int(match.group(1)),
                    'class_id': int(match.group(2)),
                    'confidence': float(match.group(3)),
                    'x': float(match.group(4)),
                    'y': float(match.group(5)),
                    'width': float(match.group(6)),
                    'height': float(match.group(7))
                }
                detections.append(detection)
                print(f"Detection: Class {detection['class_id']}, "
                      f"Confidence: {detection['confidence']:.2f}, "
                      f"Position: ({detection['x']:.2f}, {detection['y']:.2f})")
        
        return detections
    
    def close(self):
        self.ser.close()

def main():
    reader = DetectionSerialReader(port='COM3')  # Adjust port as needed
    
    try:
        print("Starting submarine detection monitor...")
        while True:
            detections = reader.read_detections()
            # Add visualization code here if needed
            
    except KeyboardInterrupt:
        print("\nStopping monitor...")
    finally:
        reader.close()

if __name__ == "__main__":
    main()