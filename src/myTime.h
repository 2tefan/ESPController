#include "time.h"

long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

const char *ntpServer = "pool.ntp.org";

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}
