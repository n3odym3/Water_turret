#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> //ArduinoJSON6
DynamicJsonDocument Input(2048);
#include "config.h"

#include <Servo.h>
Servo ServoX;
Servo ServoY;

#define RPM 200
#define DIR 15//26
#define STEP 13//25
#define EN 5//17

WiFiClient espClient;
PubSubClient client(espClient);

class StepperDriver {
  public:
    short dir_pin, step_pin, en_pin;
    int delay_step = 500;
    StepperDriver(): dir_pin(0), step_pin(0), en_pin(0) {}
    StepperDriver(short dir_pin, short step_pin, short en_pin) : dir_pin(dir_pin), step_pin(step_pin), en_pin(en_pin) {}

    void begin(int rpm) {
      pinMode(dir_pin, OUTPUT);
      pinMode(step_pin, OUTPUT);
      pinMode(en_pin, OUTPUT);

      delay_step = (60000000 / (200 * rpm)) / 2;
    }
    void enable(void) {
      digitalWrite(en_pin, LOW);
    }
    void disable(void) {
      digitalWrite(en_pin, HIGH);
    }
    void move(long steps) {
      if (steps > 0) {
        digitalWrite(dir_pin, LOW);
      }
      else {
        digitalWrite(dir_pin, HIGH);
        steps = -steps;
      }
      for (int i = 0; i < steps; i++) {
        digitalWrite(step_pin, HIGH);
        delayMicroseconds(delay_step);
        digitalWrite(step_pin, LOW);
        delayMicroseconds(delay_step);
        client.loop();
      }
    }
};
StepperDriver Pump(DIR, STEP, EN);

void callback(String topic, byte* message, unsigned int length) {
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  Serial.println(messageTemp);
  deserializeJson(Input, messageTemp);
  if (Input.containsKey("X")) {
    ServoX.write(Input["X"]);
  }
  if (Input.containsKey("Y")) {
    ServoY.write(Input["Y"]);
  }
  if (Input.containsKey("P")) {
    Pump.enable();
    Pump.move(Input["P"]);
    Pump.disable();
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(HostName);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(HostName, mqttUser, mqttPassword)) {
      Serial.println("connected");
      client.subscribe(topic_DOWN);
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  ServoX.attach(14); //D4
  ServoY.attach(12); //D4
  Pump.begin(RPM);
  Pump.disable();

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
