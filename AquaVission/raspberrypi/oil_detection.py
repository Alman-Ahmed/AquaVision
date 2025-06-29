import cv2
import numpy as np

class OilDetector:
    def __init__(self):
        self.cap = cv2.VideoCapture(0)

    def get_frame(self):
        ret, frame = self.cap.read()
        return frame if ret else None

    def detect(self, frame):
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
        lower = np.array([100, 50, 50])
        upper = np.array([140, 255, 255])
        mask = cv2.inRange(hsv, lower, upper)
        oil_pixels = cv2.countNonZero(mask)
        return oil_pixels > 500

    def release(self):
        self.cap.release()
