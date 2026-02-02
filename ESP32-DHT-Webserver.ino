#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <DHT.h>

#define DHTTYPE DHT11

const char *ssid = "Niro";
const char *password = "@Nid@Ali@";

WebServer server(80);
DHT dht(26, DHT11);

void handleRoot() {
  String html = "<!DOCTYPE html>\
<html lang='fr'>\
<head>\
    <meta charset='UTF-8'>\
    <meta http-equiv='refresh' content='4'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <title>Système de Monitoring ESP32</title>\
    <link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css' rel='stylesheet'>\
    <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css'>\
    <style>\
        body {\
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\
            min-height: 100vh;\
            padding: 20px;\
        }\
        .main-container {\
            max-width: 900px;\
            margin: 0 auto;\
        }\
        .header-card {\
            background: white;\
            border-radius: 20px;\
            padding: 30px;\
            box-shadow: 0 10px 40px rgba(0,0,0,0.1);\
            margin-bottom: 30px;\
        }\
        .header-card h1 {\
            color: #667eea;\
            font-weight: 700;\
            margin-bottom: 10px;\
        }\
        .header-card p {\
            color: #6c757d;\
            font-size: 1.1rem;\
        }\
        .sensor-card {\
            background: white;\
            border-radius: 20px;\
            padding: 40px;\
            box-shadow: 0 10px 40px rgba(0,0,0,0.1);\
            transition: transform 0.3s ease;\
            height: 100%;\
        }\
        .sensor-card:hover {\
            transform: translateY(-5px);\
        }\
        .temp-card {\
            border-left: 5px solid #ff6b6b;\
        }\
        .humidity-card {\
            border-left: 5px solid #4ecdc4;\
        }\
        .icon-wrapper {\
            width: 80px;\
            height: 80px;\
            border-radius: 50%;\
            display: flex;\
            align-items: center;\
            justify-content: center;\
            margin: 0 auto 20px;\
        }\
        .temp-icon {\
            background: linear-gradient(135deg, #ff6b6b 0%, #ee5a6f 100%);\
        }\
        .humidity-icon {\
            background: linear-gradient(135deg, #4ecdc4 0%, #44a8a0 100%);\
        }\
        .icon-wrapper i {\
            font-size: 2.5rem;\
            color: white;\
        }\
        .sensor-label {\
            color: #495057;\
            font-size: 1.2rem;\
            font-weight: 600;\
            margin-bottom: 15px;\
        }\
        .sensor-value {\
            font-size: 3.5rem;\
            font-weight: 700;\
            margin-bottom: 0;\
        }\
        .temp-value {\
            color: #ff6b6b;\
        }\
        .humidity-value {\
            color: #4ecdc4;\
        }\
        .sensor-unit {\
            font-size: 1.5rem;\
            color: #6c757d;\
            font-weight: 500;\
        }\
        .status-badge {\
            display: inline-block;\
            padding: 8px 20px;\
            background: #28a745;\
            color: white;\
            border-radius: 50px;\
            font-size: 0.9rem;\
            font-weight: 600;\
        }\
        .footer {\
            text-align: center;\
            color: white;\
            margin-top: 30px;\
            font-size: 0.9rem;\
        }\
    </style>\
</head>\
<body>\
    <div class='main-container'>\
        <div class='header-card text-center'>\
            <h1><i class='fas fa-microchip'></i> Système de Monitoring ESP32</h1>\
            <p>Surveillance en temps réel - Température & Humidité</p>\
            <span class='status-badge'><i class='fas fa-circle'></i> Connecté</span>\
        </div>\
        <div class='row g-4'>\
            <div class='col-md-6'>\
                <div class='sensor-card temp-card'>\
                    <div class='icon-wrapper temp-icon'>\
                        <i class='fas fa-thermometer-half'></i>\
                    </div>\
                    <div class='sensor-label text-center'>Température</div>\
                    <div class='text-center'>\
                        <p class='sensor-value temp-value'>";
  
  html += String(readDHTTemperature(), 1);
  html += "<span class='sensor-unit'>°C</span></p>\
                    </div>\
                </div>\
            </div>\
            <div class='col-md-6'>\
                <div class='sensor-card humidity-card'>\
                    <div class='icon-wrapper humidity-icon'>\
                        <i class='fas fa-tint'></i>\
                    </div>\
                    <div class='sensor-label text-center'>Humidité</div>\
                    <div class='text-center'>\
                        <p class='sensor-value humidity-value'>";
  
  html += String(readDHTHumidity(), 1);
  html += "<span class='sensor-unit'>%</span></p>\
                    </div>\
                </div>\
            </div>\
        </div>\
        <div class='footer'>\
            <p><i class='fas fa-sync-alt'></i> Mise à jour automatique toutes les 4 secondes</p>\
            <p>ESP32 DHT11 - Projet IoT</p>\
        </div>\
    </div>\
</body>\
</html>";

  server.send(200, "text/html", html);
}

void setup(void) {

  Serial.begin(115200);
  dht.begin();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }
  server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  delay(2);//allow the cpu to switch to other tasks
}


float readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  if (isnan(t)) {    
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }
  else {
    Serial.println(t);
    return t;
  }
}

float readDHTHumidity() {
  // Sensor readings may also be up to 2 seconds
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }
  else {
    Serial.println(h);
    return h;
  }
}
