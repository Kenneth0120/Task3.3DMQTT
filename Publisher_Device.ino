#include <WiFiNINA.h>
#include <PubSubClient.h>

// Network credentials stored in arduino_secrets.h
char ssid[] = "iiNetA59E43"; 
char pass[] = "2hAkfqsQkN"; 

// MQTT Broker details
const char* broker = "broker.emqx.io";
const int port = 1883;
const char* topic = "SIT210/wave";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Ultrasonic Sensor pins
const int trigPin = 7;
const int echoPin = 6;
long duration, distance;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  connectToWiFi();
  mqttClient.setServer(broker, port);
}

void loop() {
  if (!mqttClient.connected()) {
    reconnectToMQTT();
  }
  mqttClient.loop();

  
  digitalWrite(trigPin, LOW); // Ensure the trigPin is low
  delayMicroseconds (2); // Wait for 2 microseconds
  digitalWrite(trigPin, HIGH); // Set trigPin on HIGH state
  delayMicroseconds (10); // Wait for 10 microseconds
  digitalWrite(trigPin, LOW); // Set trigPin on LOW state

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)

  if (distance > 0 && distance < 10) { // "Wave" condition
    mqttClient.publish(topic, "Wave detected!");
    Serial.println("Wave detected!");// Message for "wave"
  } else if (distance >= 10 && distance <= 20) { // "Pat" condition
    mqttClient.publish(topic, "Pat detected!"); // Different message for "pat"
    Serial.println("Pat detected!");
  }
  delay(2000); // Debounce delay
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi.");
}

void reconnectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("PublisherDevice")) {
      Serial.println("connected");
      mqttClient.subscribe(topic); // Subscribe to topic if you want the publisher to also listen
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
