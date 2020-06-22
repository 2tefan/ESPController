// Edit this file and rename it to secrets.h

// WiFi

#define HOSTNAME "esp32"    // Hostname of ESP
#define SSID "changeme"     // SSID
#define PASSWORD "changeme" // Password

// MQTT

#define MQTT_HOST "changeme" // HOST (e.g. test.mosquitto.org)
#define MQTT_PORT 8883       // Port (8883 is default for MQTTS)
#define MQTT_USER "username" // Username
#define MQTT_PASS "password" // Password

const char *local_root_ca =
    "-----BEGIN CERTIFICATE-----\n"
    "Youneedtoputyourca(centralauthority).CRThere.Itusuallystartswith\n"
    "-----BEGIN CERTIFICATE-----andenswith-----END CERTIFICATE-----pp\n"
    "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp\n"
    "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp\n"
    "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp\n"
    "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp\n"
    "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp\n"
    "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp\n"
    "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp\n"
    "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp\n"
    "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp\n"
    "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp\n"
    "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp\n"
    "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp\n"
    "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp\n"
    "pppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppppp\n"
    "pppppppppppppppppppppppppppppppppppppppppppppppppppp=placeholder\n"
    "-----END CERTIFICATE-----\n";