#include <DHT.h> // DHT sensor library
#include <WiFi.h>
#include <PubSubClient.h>

// Define pin numbers for sensors
const int rainSensorPin = A0;   // Analog pin for rain sensor
const int mq2SensorPin = A4;    // Analog pin for MQ-2 sensor
const int dhtPin = 40;           // Digital pin for DHT sensor

const char *WIFI_SSID = "cs-mtg-room"; //your WiFi SSID
const char *WIFI_PASSWORD = "bilik703"; // your password
const char *MQTT_SERVER = "34.143.200.122"; // your VM instance public IP address
const int MQTT_PORT = 1883;
const char *MQTT_TOPIC = "iot"; // MQTT topic

// Define sensor objects
DHT dht(dhtPin, DHT11); // DHT11 sensor on digital pin 2

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{
 delay(10);
 Serial.println("Connecting to WiFi...");
 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 while (WiFi.status() != WL_CONNECTED)
 {
 delay(500);
 Serial.print(".");
 }
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
}

void setup() {
  // Start serial communication
  Serial.begin(9600);
  
  // Initialize the DHT sensor
  dht.begin();

  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
}

void reconnect()
{
 while (!client.connected())
  {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client"))
    {
      Serial.println("Connected to MQTT server");
    }
    else
    {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  // Read and print rain sensor value
  int rainValue = analogRead(rainSensorPin);
  Serial.print("Rain Sensor Value: ");
  Serial.println(rainValue);

  // Read and print temperature and humidity from DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  // Read and print MQ-2 sensor value (analog)
  int mq2Value = analogRead(mq2SensorPin);
  Serial.print("MQ-2 Sensor Value: ");
  Serial.println(mq2Value);


  char combinedPayload[100];
  snprintf(combinedPayload, sizeof(combinedPayload), "{\"temperature\": %.2f, \"humidity\": %.2f, \"mq2\": %d, \"rain\": %d}", temperature, humidity, mq2Value, rainValue);

  client.publish(MQTT_TOPIC, combinedPayload);


  // Add delay to avoid flooding the serial monitor
  delay(3000);
}