from serial_reader import SerialReader
from oil_detection import OilDetector
from logger import Logger
from trash_detection import TrashDetector
import cv2

def main():
    serial_reader = SerialReader('/dev/ttyUSB0', 9600)
    logger = Logger('sensor_log.csv')
    oil_detector = OilDetector()
    trash_detector = TrashDetector()

    last_oil_state = None
    last_trash_state = None
    last_dirtiness = None

    while True:
        data = serial_reader.read_data()
        dirtiness = 0.0
        if data:
            logger.log(data)
            print(data)
            # Calculate dirtiness: combine normalized turbidity and trash detection
            turbidity = data['turbidity']
            # Assume 0-1000 NTU, normalize to 0-1
            turbidity_score = min(max(turbidity / 1000.0, 0.0), 1.0)
            dirtiness = turbidity_score
        frame = oil_detector.get_frame()
        trash_frame = trash_detector.get_frame()
        oil_detected = False
        trash_detected = False
        if frame is not None:
            oil_detected = oil_detector.detect(frame)
        if trash_frame is not None:
            trash_detected, trash_annotated = trash_detector.detect(trash_frame)
            if trash_detected:
                dirtiness = max(dirtiness, 0.8)  # If trash, dirtiness is at least 80%

        # Compose dashboard window
        if frame is not None:
            label = "Oil Detected" if oil_detected else "Clean"
            color = (0, 0, 255) if oil_detected else (0, 255, 0)
            cv2.putText(frame, label, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, color, 2)
            trash_label = "Trash Detected" if trash_detected else "No Trash"
            trash_color = (0, 0, 255) if trash_detected else (0, 255, 0)
            cv2.putText(frame, trash_label, (10, 70), cv2.FONT_HERSHEY_SIMPLEX, 1, trash_color, 2)
            dirtiness_label = f"Dirtiness: {int(dirtiness*100)}%"
            cv2.putText(frame, dirtiness_label, (10, 110), cv2.FONT_HERSHEY_SIMPLEX, 1, (128, 0, 128), 2)
            cv2.imshow("AquaVision AI", frame)

        # Send command to ESP32 if oil detection state changes
        if oil_detected != last_oil_state:
            if oil_detected:
                serial_reader.send_command("STOP")
            else:
                serial_reader.send_command("START")
            last_oil_state = oil_detected

        # Optional: Manual override via keyboard
        key = cv2.waitKey(1) & 0xFF
        if key == ord('q'):
            break
        elif key == ord('s'):
            serial_reader.send_command("STOP")
        elif key == ord('r'):
            serial_reader.send_command("START")
        elif key == ord('e'):
            serial_reader.send_command("ESC,1600,1600")

    oil_detector.release()
    trash_detector.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
