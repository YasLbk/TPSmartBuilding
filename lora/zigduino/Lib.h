#include <TheThingsNetwork.h>
#include <CayenneLPP.h>

/* Copy KEY.h.example to KEY.h and edit keys */
#include "KEY.h"

long randNumber;

#define freqPlan TTN_FP_EU868
#define serialLora Serial1
#define serialDebug Serial
#define SFF 7
#define SFW 7

/* Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915 */
#define freqPlan TTN_FP_EU868

/* TTN */
TheThingsNetwork ttn(serialLora, serialDebug, freqPlan, SFF);
CayenneLPP lpp(51);

/* EEPROM */
char cmd_buffer[256];

/***************************************/
/* Sensors */
#define SENSOR_SHT A0
#define SENSOR_LIGHT A1
#define SENSOR_SOUND A2
#define SENSOR_PIR 4

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN SENSOR_SHT
#define DHTTYPE DHT22
#define BUFSIZE 4096
DHT_Unified dht(DHTPIN, DHTTYPE);
sensors_event_t event_temp;
sensors_event_t event_hum;

float no_zigduino;
float h;
float t;
int pir;
int light;
int sound;

int counter = 0;