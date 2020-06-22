#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include "myTime.h"
#include "ESP_DEFINES.h"
#include "helper.h"
#include "ledHandler.h"
#include "networking.h"

#define NUM_ADC_READ 11
#define MIN_TIME_BETWEEN_BTN_PRESS_MS 250
#define INFO_ONLINE "online"
#define INFO_OFFLINE "offline"

#define HA_VERSION "0.1.0"
#define HA_MANUFACTURER "Schober Inc"
#define HA_MODEL_POTI "Potentiometer (12-Bit)"
#define HA_MODEL_BTN "Simple Button"
#define HA_POTI_ICON "mdi:circular-saw"

bool firstRun = true;
short lastPotiValues[LENGHT_POTIS];
String queuedMQTT[15][2];
unsigned long lastPress[LENGHT_BUTTONS];

String identifier = "SSMC-" + String((unsigned long)(ESP.getEfuseMac()));

#define MQTT_SUB_TOPIC_LED_COLOR "bedroom/controller/color"
#define MQTT_SUB_TOPIC_HA_STATUS "hass/status"
#define MQTT_PUB_TOPIC_AVAILABILITY "bedroom/controller/status"

#define MQTT_PUB_TOPIC_BTN "homeassistant/device_automation/bedroom/btn"
#define MQTT_PUB_TOPIC_POTI "homeassistant/sensor/bedroom/poti"
#define MQTT_PUB_TOPIC_POTI_AVAILABILITY MQTT_PUB_TOPIC_AVAILABILITY

#define MQTT_PUB_TOPIC_CONFIG "homeassistant/device_automation/bedroom/controller/config"
#define MQTT_WILL_TOPIC MQTT_PUB_TOPIC_AVAILABILITY
#define MQTT_WILL_QOS 0
#define MQTT_WILL_RETAIN true
#define MQTT_WILL_MESSAGE INFO_OFFLINE

time_t now;

void setup_time()
{
  set_LED(INFO_LED, COLOR_OBTAINING_TIME);
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  set_LED(INFO_LED, COLOR_EVERYTHING_WORKING);
}

void setup_ha_buttons()
{
  for (size_t i = 0; i < LENGHT_BUTTONS; i++)
  {
    String name = "Button " + String(i);
    String uniqueIdentifier = identifier + "-btn" + String(i);
    String outputTopic = MQTT_PUB_TOPIC_BTN + String(i);
    String configTopic = outputTopic + "/config";

    const size_t capacity = JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(5);
    DynamicJsonDocument doc(capacity);

    doc["automation_type"] = "trigger";
    doc["topic"] = outputTopic.c_str();
    doc["type"] = "button_short_press";
    doc["subtype"] = "button_1";

    JsonObject device = doc.createNestedObject("device");
    device["identifiers"] = uniqueIdentifier.c_str();
    device["name"] = name.c_str();

    device["manufacturer"] = HA_MANUFACTURER;
    device["model"] = HA_MODEL_BTN;
    device["sw_version"] = HA_VERSION;

    String payload;
    serializeJson(doc, payload);

    client.publish(configTopic.c_str(), payload.c_str());
  }
}

void setup_ha_potis()
{
  for (size_t i = 0; i < LENGHT_POTIS; i++)
  {
    String name = "Potentiometer " + String(i);
    String uniqueIdentifier = identifier + "-poti" + String(i);
    String baseTopic = MQTT_PUB_TOPIC_POTI + String(i);
    String configTopic = baseTopic + "/config";
    String stateTopic = baseTopic + "/state";

    const size_t capacity = JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(5);
    DynamicJsonDocument doc(capacity);

    doc["state_topic"] = stateTopic.c_str();
    doc["availability_topic"] = MQTT_PUB_TOPIC_POTI_AVAILABILITY;
    doc["name"] = name.c_str();
    doc["unique_id"] = uniqueIdentifier.c_str();
    doc["icon"] = HA_POTI_ICON;

    JsonObject device = doc.createNestedObject("device");
    device["identifiers"] = uniqueIdentifier.c_str();
    device["name"] = name.c_str();

    device["manufacturer"] = HA_MANUFACTURER;
    device["model"] = HA_MODEL_POTI;
    device["sw_version"] = HA_VERSION;

    String payload;
    serializeJson(doc, payload);

    client.publish(configTopic.c_str(), payload.c_str());
  }

  client.publish(MQTT_PUB_TOPIC_POTI_AVAILABILITY, INFO_ONLINE, true);
}

void setup_homeassistant()
{
  setup_ha_buttons();
  setup_ha_potis();
}

void mqtt_connect()
{
  set_LED(INFO_LED, COLOR_NO_MQTT_CONNECTION);

  while (!client.connected())
  {
    Serial.println("MQTT connecting...");
    if (client.connect(HOSTNAME, MQTT_USER, MQTT_PASS, MQTT_WILL_TOPIC, MQTT_WILL_QOS, MQTT_WILL_RETAIN, MQTT_WILL_MESSAGE))
    {
      Serial.println("Subscribing to TOPICs: ");
      Serial.println(MQTT_SUB_TOPIC_LED_COLOR);
      Serial.println(MQTT_SUB_TOPIC_HA_STATUS);
      client.subscribe(MQTT_SUB_TOPIC_LED_COLOR);
      client.subscribe(MQTT_SUB_TOPIC_HA_STATUS);
    }
    else
    {
      Serial.print("failed, status code = \"");
      Serial.print(client.state());
      Serial.println("\" - Try again in 2 seconds");
      delay(2000);
    }
  }

  Serial.println("MQTT connected!");
  set_LED(INFO_LED, COLOR_EVERYTHING_WORKING);

  setup_homeassistant();
}

void receivedCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Received [");
  Serial.print(topic);
  Serial.print("]: ");
  String payloadString = "";

  for (int i = 0; i < length; i++)
  {
    payloadString += String(char(payload[i]));
  }

  Serial.println(payloadString);

  if (String(topic).equals(MQTT_SUB_TOPIC_HA_STATUS))
  {
    if (payloadString.equals(INFO_ONLINE))
    {
      Serial.println("HA is online again!");
      set_LED(INFO_LED, COLOR_EVERYTHING_WORKING);
      setup_homeassistant();
    }
    else if (payloadString.equals(INFO_OFFLINE))
    {
      Serial.println("HA is offline!");
      set_LED(INFO_LED, COLOR_HA_OFFLINE);
    }
    else
    {
      Serial.println("Something happend with HA!");
    }
  }
  else if (String(topic).equals(MQTT_SUB_TOPIC_LED_COLOR))
  {
    size_t color = (int)strtol(payloadString.c_str(), NULL, 16);

    if (0x0 <= color && color <= 0xFFFFFF)
    {
      Serial.println(color);
      set_LED(NOTIFICATION_LED, color);
    }
    else
    {
      Serial.println("Invalid color!");
    }
  }
}

void setup_mqtt()
{
  firstRun = true;
  Serial.println(identifier);
  net.setCACert(local_root_ca);
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(receivedCallback);
  client.setBufferSize(512);
  mqtt_connect();
}

void IRAM_ATTR isr(int buttonIndex, String mode)
{
  for (size_t i = 0; i < sizeof(queuedMQTT) / sizeof(*queuedMQTT); i++)
  {
    if (queuedMQTT[i][0] == "")
    {
      queuedMQTT[i][0] = String(buttonIndex);
      queuedMQTT[i][1] = mode;
      //Serial.println("Running on core " + String(xPortGetCoreID()));
      return;
    }
  }

  Serial.println("MQTT Buffer full!");
}

void IRAM_ATTR isr_r(int buttonIndex)
{
  if (millis() > lastPress[buttonIndex] + MIN_TIME_BETWEEN_BTN_PRESS_MS)
  {
    isr(buttonIndex, "PRESSED");
    lastPress[buttonIndex] = millis();
  }
  else
  {
    Serial.println("Bounce detected!");
  }
}

void IRAM_ATTR isr_0()
{
  isr_r(0);
}
void IRAM_ATTR isr_1()
{
  isr_r(1);
}
void IRAM_ATTR isr_2()
{
  isr_r(2);
}
void IRAM_ATTR isr_3()
{
  isr_r(3);
}
void IRAM_ATTR isr_4()
{
  isr_r(4);
}
void IRAM_ATTR isr_5()
{
  isr_r(5);
}
void IRAM_ATTR isr_6()
{
  isr_r(6);
}
void IRAM_ATTR isr_7()
{
  isr_r(7);
}
void IRAM_ATTR isr_8()
{
  isr_r(8);
}
void IRAM_ATTR isr_9()
{
  isr_r(9);
}
void IRAM_ATTR isr_10()
{
  isr_r(10);
}

TaskHandle_t TaskA;

void setup_pins()
{
  // Pulldown all buttones
  for (size_t i = 0; i < LENGHT_BUTTONS; i++)
  {
    pinMode(buttons[i], INPUT_PULLDOWN);
  }

  // Attach ISRs to Buttons
  void (*isrArray[LENGHT_BUTTONS])() = {isr_0, isr_1, isr_2, isr_3, isr_4, isr_5, isr_6, isr_7, isr_8, isr_9, isr_10};

  for (size_t i = 0; i < LENGHT_BUTTONS; i++)
  {
    attachInterrupt(digitalPinToInterrupt(buttons[i]), isrArray[i], RISING);
  }
}

void setup()
{
  Serial.begin(115200);

  setup_LEDs();
  setup_pins();
  setup_wifi();
  setup_time();
  setup_mqtt();
  Serial.println("Running on core " + String(xPortGetCoreID()));
}

void loop()
{
  now = time(nullptr);
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Lost WiFi-Connection!");
    setup_wifi();
  }

  if (!client.connected())
  {
    Serial.println("Lost MQTT-Connection!");
    mqtt_connect();
  }

  client.loop();
  delay(250);

  for (size_t i = 0; i < sizeof(queuedMQTT) / sizeof(*queuedMQTT); i++)
  {
    if (queuedMQTT[i][0] != "")
    {
      String topic = MQTT_PUB_TOPIC_BTN + queuedMQTT[i][0];
      String payload = queuedMQTT[i][1];

      client.publish(topic.c_str(), payload.c_str());
      queuedMQTT[i][0] = "";
      queuedMQTT[i][1] = "";
    }
  }

  short tempPotiValues[NUM_ADC_READ];
  for (size_t i = 0; i < sizeof(potis) / sizeof(*potis); i++)
  {
    for (size_t j = 0; j < NUM_ADC_READ; j++)
    {
      tempPotiValues[j] = analogRead(potis[i]);
      delay(10);
    }

    short potiValue = findMedian(tempPotiValues, NUM_ADC_READ);

    if (isADifferentValue(potiValue, lastPotiValues[i], 50))
    {
      if (!firstRun) // Do not publish on first startup - only on change
      {
        String potiString = String(potiValue);
        String topic = MQTT_PUB_TOPIC_POTI + String(i) + String("/state");

        client.publish(topic.c_str(), potiString.c_str());
      }

      lastPotiValues[i] = potiValue;
    }
  }

  firstRun = false;
}