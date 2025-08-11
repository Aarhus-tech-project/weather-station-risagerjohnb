#include <WiFiS3.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

// WiFi credentials
const char* ssid = "h4prog";
const char* password = "1234567890";

// MQTT broker
const char* mqtt_server = "192.168.104.11"; // Use your own IP later

WiFiClient wifiClient;
PubSubClient client(wifiClient);
Adafruit_BME280 bme;

void setup_wifi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
		Serial.print(WiFi.status());
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
		delay(1000);
		Serial.print(WiFi.status());
    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}

void testConnectionToServer() {
  const char* host = "192.168.104.11";  // Replace with your server IP
  const int port = 1883;              // MQTT port

  Serial.print("Testing connection to ");
  Serial.print(host);
  Serial.print(":");
  Serial.println(port);

  if (wifiClient.connect(host, port)) {
    Serial.println("Server is reachable (TCP connection succeeded).");
    wifiClient.stop();
  } else {
    Serial.println("Failed to reach server (TCP connection failed).");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection... ");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, you can publish or subscribe
      client.subscribe("weatherstation/command");  // optional
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state()); // Gives specific error code
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


void setup() {
	Serial.begin(9600);

	if (!bme.begin(0x76)) {
		Serial.println("Could not find a valid BME280 sensor!");
		while (1);
	}

	setup_wifi();
	client.setServer(mqtt_server, 1883);
}

const unsigned long interval = 86400000UL / 10; // 24h in ms divided by 10 = 2.4 hours interval
unsigned long previousMillis = 0;

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    String payload = "{";
    payload += "\"temperature\":" + String(bme.readTemperature(), 2) + ",";
    payload += "\"pressure\":" + String(bme.readPressure() / 100.0F, 2) + ",";
    payload += "\"humidity\":" + String(bme.readHumidity(), 2);
    payload += "}";

    client.publish("weather/data", payload.c_str());
    Serial.println("Published: " + payload);
  }
}

