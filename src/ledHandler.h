#define COLOR_EVERYTHING_WORKING 0x0
#define COLOR_NO_WIFI 0xff0000
#define COLOR_NO_MQTT_CONNECTION 0xffb300
#define COLOR_OBTAINING_TIME 0x00ffd4
#define COLOR_HA_OFFLINE 0xff7e00

#define INFO_LED 0
#define NOTIFICATION_LED 1

const int ledc0[LEDC_CHANNEL_LENGHT] = {LEDC0_CHANNEL_RED, LEDC0_CHANNEL_GREEN, LEDC0_CHANNEL_BLUE};
const int ledc1[LEDC_CHANNEL_LENGHT] = {LEDC1_CHANNEL_RED, LEDC1_CHANNEL_GREEN, LEDC1_CHANNEL_BLUE};
const int *leds[] = {ledc0, ledc1};

TaskHandle_t TaskLEDs;

void set_LED_RGB(__UINT_LEAST8_TYPE__ led, __UINT_LEAST8_TYPE__ ledR, __UINT_LEAST8_TYPE__ ledG, __UINT_LEAST8_TYPE__ ledB)
{
    //Serial.printf("%d-%d-%d\n", int(ledR), int(ledG), int(ledB));
    ledcWrite(leds[led][0], ledR);
    ledcWrite(leds[led][1], ledG);
    ledcWrite(leds[led][2], ledB);
}

void set_LED(__UINT_LEAST8_TYPE__ led, size_t color)
{
    set_LED_RGB(led, (color & 0xff0000) >> 16, (color & 0x00ff00) >> 8, (color & 0x0000ff));
}

void setup_LEDs()
{
    // LEDC for both RGB-LEDs
    ledcAttachPin(RGB0_RED, LEDC0_CHANNEL_RED);
    ledcAttachPin(RGB0_GREEN, LEDC0_CHANNEL_GREEN);
    ledcAttachPin(RGB0_BLUE, LEDC0_CHANNEL_BLUE);

    ledcAttachPin(RGB1_RED, LEDC1_CHANNEL_RED);
    ledcAttachPin(RGB1_GREEN, LEDC1_CHANNEL_GREEN);
    ledcAttachPin(RGB1_BLUE, LEDC1_CHANNEL_BLUE);

    for (size_t i = 0; i < LEDC_CHANNEL_LENGHT; i++)
    {
        ledcSetup(ledc0[i], LEDC_FREQ_HZ, LEDC_RESOLUTION_BITS);
        ledcSetup(ledc1[i], LEDC_FREQ_HZ, LEDC_RESOLUTION_BITS);
    }
}