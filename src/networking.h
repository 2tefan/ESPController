WiFiClientSecure net;
PubSubClient client(net);

void set_LED(__UINT_LEAST8_TYPE__ led, size_t color);

void setup_wifi()
{
    set_LED(INFO_LED, COLOR_NO_WIFI);
    Serial.print("\nConnecting to ");
    Serial.println(SSID);

    WiFi.begin(SSID, PASSWORD); // Connect to network
    delay(1000);
    WiFi.reconnect();

    while (WiFi.status() != WL_CONNECTED)
    { // Wait for connection
        delay(500);
        Serial.print(".");
    }

    set_LED(INFO_LED, COLOR_EVERYTHING_WORKING);
    Serial.println();
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}