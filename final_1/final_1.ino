#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h> 

const int ds18b20Pin = D1;

const char* ssid = "PoleDeVinci_IFT";       
const char* password = "*c.r4UV@VfPn_0"; 

OneWire oneWire(ds18b20Pin);
DallasTemperature sensors(&oneWire);

WebSocketsServer webSocket = WebSocketsServer(81);

const int motorPin = D2; //pin pour le moteaur
const int ledPinBleu = D3;
const int ledPinRouge = D4;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password); //On se connecte au Wifi
    Serial.println("Connecting to WiFi...");

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    pinMode(motorPin, OUTPUT);
    digitalWrite(motorPin, LOW);
    pinMode(ledPinBleu, OUTPUT);
    digitalWrite(ledPinBleu, LOW);
    pinMode(ledPinRouge, OUTPUT);
    digitalWrite(ledPinRouge, LOW);

    webSocket.onEvent(webSocketEvent);
    webSocket.begin();
    

    server.begin();
}

void loop() {
    webSocket.loop();
    sensors.requestTemperatures(); 
    float temperatureC = sensors.getTempCByIndex(0);
    String tempMessage = String("{\"temperature\":") + temperatureC + "}";
    webSocket.broadcastTXT(tempMessage);
    delay(100);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_TEXT) {
        String message = String((char *)payload);
        Serial.println("Message received: " + message);

        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, message);
        if (error) {
            Serial.println("Failed to parse JSON");
            return;
        }

        const char* command = doc["command"];
        
        if (String(command) == "FORWARD") {
            digitalWrite(motorPin, HIGH); 
        } else if (String(command) == "STOP") {
            digitalWrite(motorPin, LOW);
        }

        if (String(command) == "BLUE") {
            digitalWrite(ledPinBleu, HIGH); 
        } else if (String(command) == "RED") {
            digitalWrite(ledPinRouge, HIGH); 
        }
        else if (String(command)=="OFF"){
            digitalWrite(ledPinRouge, LOW);
            digitalWrite(ledPinBleu, LOW);

        }
    }
}
