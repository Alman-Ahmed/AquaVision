import serial

class SerialReader:
    def __init__(self, port, baudrate):
        self.ser = serial.Serial(port, baudrate, timeout=1)

    def read_data(self):
        line = self.ser.readline().decode().strip()
        if line:
            try:
                ph, turbidity, temp, batt = map(float, line.split(','))
                return {'ph': ph, 'turbidity': turbidity, 'temp': temp, 'batt': batt}
            except Exception:
                return None
        return None

    def send_command(self, cmd):
        self.ser.write((cmd.strip() + '\n').encode())
