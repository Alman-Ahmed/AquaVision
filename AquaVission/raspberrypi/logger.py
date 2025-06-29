import csv
from datetime import datetime

class Logger:
    def __init__(self, filename):
        self.filename = filename
        with open(self.filename, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['timestamp', 'pH', 'turbidity', 'temperature', 'battery'])

    def log(self, data):
        with open(self.filename, 'a', newline='') as f:
            writer = csv.writer(f)
            writer.writerow([datetime.now().isoformat(), data['ph'], data['turbidity'], data['temp'], data['batt']])
