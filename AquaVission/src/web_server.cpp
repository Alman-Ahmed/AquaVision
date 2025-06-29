#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "sensors.h"
#include "battery_monitor.h"
#include "esc_control.h"

// Simulated values for oil film and dirtiness (replace with real sensor/serial input if available)
bool oil_film_detected = false;
float water_dirtiness = 0.0; // 0 (clean) to 1 (very dirty)

// Replace with your network credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

AsyncWebServer server(80);

String processor(const String& var) {
  if (var == "PH") return String(read_ph(), 2);
  if (var == "TURBIDITY") return String(read_turbidity(), 2);
  if (var == "TEMP") return String(read_temperature(), 2);
  if (var == "BATT") return String(read_battery_voltage(), 2);
  if (var == "CHARGING") {
    static float lastVoltage = 0;
    float batt = read_battery_voltage();
    String state = (batt > lastVoltage + 0.01) ? "YES" : "NO";
    lastVoltage = batt;
    return state;
  }
  if (var == "ESC1") return String(get_esc1_throttle());
  if (var == "ESC2") return String(get_esc2_throttle());
  if (var == "OILFILM") return oil_film_detected ? "Detected" : "None";
  if (var == "DIRTINESS") return String(water_dirtiness * 100, 1) + "%";
  return "";
}

void start_web_server() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"ph\":" + String(read_ph(), 2) + ",";
    json += "\"turbidity\":" + String(read_turbidity(), 2) + ",";
    json += "\"temp\":" + String(read_temperature(), 2) + ",";
    json += "\"batt\":" + String(read_battery_voltage(), 2) + ",";
    json += "\"charging\":\"" + processor("CHARGING") + "\",";
    json += "\"esc1\":" + String(get_esc1_throttle()) + ",";
    json += "\"esc2\":" + String(get_esc2_throttle()) + ",";
    json += "\"oilfilm\":\"" + processor("OILFILM") + "\",";
    json += "\"dirtiness\":\"" + processor("DIRTINESS") + "\"";
    json += "}";
    request->send(200, "application/json", json);
  });

  server.begin();
}

// HTML page as a raw string
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>AquaVision Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; background: #e0f7fa; color: #222; }
    .card { background: #fff; padding: 20px; margin: 30px auto; border-radius: 10px; max-width: 400px; box-shadow: 0 2px 8px #aaa; }
    h2 { color: #00796b; }
    .data { font-size: 1.2em; margin: 10px 0; }
    .label { color: #555; }
  </style>
</head>
<body>
  <div class="card">
    <h2>AquaVision Dashboard</h2>
    <div class="data"><span class="label">pH:</span> <span id="ph">--</span></div>
    <div class="data"><span class="label">Turbidity:</span> <span id="turbidity">--</span> NTU</div>
    <div class="data"><span class="label">Temperature:</span> <span id="temp">--</span> &deg;C</div>
    <div class="data"><span class="label">Battery:</span> <span id="batt">--</span> V</div>
    <div class="data"><span class="label">Charging:</span> <span id="charging">--</span></div>
    <div class="data"><span class="label">ESC1:</span> <span id="esc1">--</span> &nbsp; <span class="label">ESC2:</span> <span id="esc2">--</span></div>
    <div class="data"><span class="label">Oil Film:</span> <span id="oilfilm">--</span></div>
    <div class="data"><span class="label">Dirtiness:</span> <span id="dirtiness">--</span></div>
  </div>
  <script>
    function fetchData() {
      fetch('/data').then(r => r.json()).then(data => {
        document.getElementById('ph').textContent = data.ph;
        document.getElementById('turbidity').textContent = data.turbidity;
        document.getElementById('temp').textContent = data.temp;
        document.getElementById('batt').textContent = data.batt;
        document.getElementById('charging').textContent = data.charging;
        document.getElementById('esc1').textContent = data.esc1;
        document.getElementById('esc2').textContent = data.esc2;
        document.getElementById('oilfilm').textContent = data.oilfilm;
        document.getElementById('dirtiness').textContent = data.dirtiness;
      });
    }
    setInterval(fetchData, 2000);
    fetchData();
  </script>
</body>
</html>
)rawliteral;
