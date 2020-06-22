#include "ArduinoSort.h"

void printArray(short *arr, int size)
{
    for (size_t i = 0; i < size - 1; i++)
    {
        Serial.print(arr[i]);
        Serial.print("-");
    }

    Serial.print(arr[size - 1]);
    Serial.println();
}

short findMedian(short *arr, int size)
{
    sortArray(arr, size);
    return arr[size/2];
}

bool isADifferentValue(short value1, short value2, byte tolerance)
{
    return value1 - tolerance > value2 || value2 > value1 + tolerance;
}