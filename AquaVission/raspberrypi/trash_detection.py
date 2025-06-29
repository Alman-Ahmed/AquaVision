import cv2
import numpy as np
try:
    from tflite_runtime.interpreter import Interpreter
except ImportError:
    Interpreter = None

class TrashDetector:
    def __init__(self, model_path='model/trash_detector.tflite'):
        self.model = None
        self.interpreter = None
        self.input_details = None
        self.output_details = None
        self.input_shape = None
        if Interpreter is not None:
            try:
                self.interpreter = Interpreter(model_path=model_path)
                self.interpreter.allocate_tensors()
                self.input_details = self.interpreter.get_input_details()
                self.output_details = self.interpreter.get_output_details()
                self.input_shape = self.input_details[0]['shape'][1:3]
            except Exception as e:
                print(f"Could not load TFLite model: {e}")
        self.cap = cv2.VideoCapture(0)

    def get_frame(self):
        ret, frame = self.cap.read()
        return frame if ret else None

    def detect(self, frame):
        if self.interpreter is not None and self.input_shape is not None:
            img = cv2.resize(frame, tuple(self.input_shape))
            img = np.expand_dims(img, axis=0).astype(np.float32) / 255.0
            self.interpreter.set_tensor(self.input_details[0]['index'], img)
            self.interpreter.invoke()
            boxes = self.interpreter.get_tensor(self.output_details[0]['index'])[0]
            classes = self.interpreter.get_tensor(self.output_details[1]['index'])[0]
            scores = self.interpreter.get_tensor(self.output_details[2]['index'])[0]
            for i, score in enumerate(scores):
                if score > 0.5:
                    # Optionally draw bounding box
                    ymin, xmin, ymax, xmax = boxes[i]
                    h, w, _ = frame.shape
                    (left, top, right, bottom) = (int(xmin * w), int(ymin * h), int(xmax * w), int(ymax * h))
                    cv2.rectangle(frame, (left, top), (right, bottom), (0, 0, 255), 2)
                    return True, frame
            return False, frame
        else:
            # Fallback: simple contour-based stub
            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            _, thresh = cv2.threshold(gray, 200, 255, cv2.THRESH_BINARY)
            contours, _ = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            trash_detected = False
            for cnt in contours:
                area = cv2.contourArea(cnt)
                if area > 1000:
                    trash_detected = True
                    x, y, w, h = cv2.boundingRect(cnt)
                    cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 0, 255), 2)
            return trash_detected, frame

    def release(self):
        self.cap.release()
