#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

// Wi-Fi credentials
const char* ssid = "OTR DASH";
const char* password = "12345678";

// Create AsyncWebServer on port 80
AsyncWebServer server(80);

// Variables to display
float speed = 0;
float batteryTemp = 0;
float acceleration = 0;
float torque = 0;
float voltage = 0;
float current = 0;
float latitude = 0;
float longitude = 0;

// Function to send commands to Nextion display
void sendToNextion(String command) {
  Serial.print(command);
  Serial.write(0xFF); // End of command
  Serial.write(0xFF); // End of command
  Serial.write(0xFF); // End of command
}

// Function to return sensor data as JSON
String getSensorData() {
  return String("{\"speed\":") + String(speed) +
         ", \"batteryTemp\":" + String(batteryTemp) +
         ", \"acceleration\":" + String(acceleration) +
         ", \"torque\":" + String(torque) +
         ", \"voltage\":" + String(voltage) +
         ", \"current\":" + String(current) +
         ", \"latitude\":" + String(latitude) +
         ", \"longitude\":" + String(longitude) + "}";
}

// HTML Webpage Template with AJAX for real-time updates
String htmlPage() {
  return String("<!DOCTYPE html><html><head><meta charset='UTF-8'>") +
         "<meta name='viewport' content='width=device-width, initial-scale=1.0'>" +
         "<title>ESP32 Sensor Dashboard</title>" +
         "<style>" +
         "body { font-family: Arial, sans-serif; background-color: #f0f0f0; color: #333; }" +
         ".container { padding: 20px; max-width: 600px; margin: auto; background-color: white; box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.1); }" +
         "h1 { color: #0d544a; }" +
         ".value { font-size: 20px; padding: 10px; margin: 5px 0; background-color: #1ea778; color: white; border-radius: 5px; }" +
         "</style>" +
         "<script>" +
         "function fetchData() {" +
         "  fetch('/sensorData').then(response => response.json()).then(data => {" +
         "    document.getElementById('speed').innerHTML = 'Speed: ' + data.speed + ' km/h';" +
         "    document.getElementById('batteryTemp').innerHTML = 'Battery Temperature: ' + data.batteryTemp + ' °C';" +
         "    document.getElementById('acceleration').innerHTML = 'Acceleration: ' + data.acceleration + ' m/s²';" +
         "    document.getElementById('torque').innerHTML = 'Torque: ' + data.torque + ' Nm';" +
         "    document.getElementById('voltage').innerHTML = 'Voltage: ' + data.voltage + ' V';" +
         "    document.getElementById('current').innerHTML = 'Current: ' + data.current + ' A';" +
         "    document.getElementById('latitude').innerHTML = 'Latitude: ' + data.latitude;" +
         "    document.getElementById('longitude').innerHTML = 'Longitude: ' + data.longitude;" +
         "  });" +
         "}" +
         "setInterval(fetchData, 100);" + 
         "</script></head><body>" +
         "<div class='container'>" +
         "<h1>ESP32 Sensor Dashboard</h1>" +
         "<div id='speed' class='value'>Speed: </div>" +
         "<div id='batteryTemp' class='value'>Battery Temperature: </div>" +
         "<div id='acceleration' class='value'>Acceleration: </div>" +
         "<div id='torque' class='value'>Torque: </div>" +
         "<div id='voltage' class='value'>Voltage: </div>" +
         "<div id='current' class='value'>Current: </div>" +
         "<div id='latitude' class='value'>Latitude: </div>" +
         "<div id='longitude' class='value'>Longitude: </div>" +
         "</div></body></html>";
}

void setup() {
  // Start Serial for debugging
  Serial.begin(38400);

  // Initialize Serial2 for the Nextion display


  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) { delay(10); }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Start Wi-Fi as an Access Point
  WiFi.softAP(ssid, password);

  // Print AP IP Address
  Serial.println("Wi-Fi Access Point Created");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Define routes for the web server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", htmlPage());
  });

  server.on("/sensorData", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "application/json", getSensorData());
  });

  // Start the server
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  // Get new sensor events with the readings
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Update acceleration based on Y-axis
  acceleration = a.acceleration.y;

  // Simulate sensor values for testing (remove this section in real use)
  speed += 1;
  if (speed > 260) speed = 0;

  batteryTemp += 0.1;
  if (batteryTemp > 50) batteryTemp = 0;

  torque += 2;
  if (torque > 100) torque = 0;

  voltage += 0.5;
  if (voltage > 12) voltage = 0;

  current += 0.1;
  if (current > 5) current = 0;

  latitude += 0.1;
  if (latitude > 90) latitude = -90;

  longitude += 0.1;
  if (longitude > 180) longitude = -180;
float angle=300+(2.134*speed);
if (angle>360) angle-=360;
  // Update Nextion Display
  sendToNextion("page1.z0.val=" + String(static_cast<int>(angle)));
  sendToNextion("page1.speed.txt=\"" + String(speed) + "\"");
  sendToNextion("page2.torque.txt=\"" + String(torque) + "\"");
  sendToNextion("page3.batteryTemp.txt=\"" + String(batteryTemp) + "\"");
  sendToNextion("page4.acceleration.txt=\"" + String(acceleration) + "\"");
  sendToNextion("page5.latitude.txt=\"" + String(latitude) + "\"");
  sendToNextion("page5.longitude.txt=\"" + String(longitude) + "\"");
  sendToNextion("page5.longitude.txt=\"" + String(longitude) + "\"");
  sendToNextion("page6.current.txt=\"" + String(current) + "\"");
  sendToNextion("page7.voltage.txt=\"" + String(voltage) + "\"");
  sendToNextion("page6.current.txt=\"" + String(current) + "\"");



  sendToNextion("page8.torque.txt=\"" + String(torque) + "\"");
  sendToNextion("page8.batteryTemp.txt=\"" + String(batteryTemp) + "\"");
  sendToNextion("page8.acceleration.txt=\"" + String(acceleration) + "\"");
  sendToNextion("page8.latitude.txt=\"" + String(latitude) + "\"");
  sendToNextion("page8.longitude.txt=\"" + String(longitude) + "\"");
  sendToNextion("page8.longitude.txt=\"" + String(longitude) + "\"");
  sendToNextion("page8.current.txt=\"" + String(current) + "\"");
  sendToNextion("page8.voltage.txt=\"" + String(voltage) + "\"");
  sendToNextion("page8.current.txt=\"" + String(current) + "\"");
  delay(100); // Delay for stability
}
