#include "LIB.h"
#include <avr/eeprom.h>

void pinsInit()
{
  pinMode(SENSOR_PIR, INPUT);
}


char line[BUFSIZE] = {0};
char *token = NULL;
int nb_data = 0, acc = 0, nb_acc = 0, beg_channel = 0;

/**
 * @brief Associate a Zigduino with it's first channel.
 * 
 * Every Zigduino has 5 channel (1 for each data: temp hum light sound and pir)
 * This structure only gives the first one.
 * 
 * Channels available for the Zigduino:
 *  - first_channel
 *  - first_channel + 1
 *  - first_channel + 2
 *  - first_channel + 3
 *  - first_channel + 4
 */
struct zigduino {
  int name;
  int first_channel;
};

/**
 * @brief Array of Zigduino used in this experience.
 */
const struct zigduino channels[10] = {
  {14, 1},
  {33, 6},
  {34, 11},
  {48, 16},
  {49, 21},
  {50, 26},
  {51, 31},
  {52, 36},
  {53, 41},
  {81, 46},
};

/**
 * @brief Get the first channel associated to a Zigduino
 * 
 * @param no_zigduino the id number of a zigduino
 * @return const int  the nimber of the channel associated to the Zigduino or -1 if the Zigduino number is not found
 */
const int getChannel(int no_zigduino) {
  for (int i = 0; i < 10; i++)
  {
    if (channels[i].name == no_zigduino)
          return channels[i].first_channel;
  }
  return -1;
}

// the setup routine runs once when you press reset:
void setup()
{
  pinsInit();
  dht.begin();

  serialDebug.begin(57600);
  /* Wait a maximum of 10s for Serial Monitor */
  while (!serialDebug && millis() < 10000)
    ;
  serialLora.begin(57600);

  RN2483_WAKEUP();
  serialDebug.println("Starting our zigduino ");
  sendCmd("sys factoryRESET");
  delay(1000);
  sendCmd("sys get hweui");
  delay(1000);
  sendCmd("sys get ver");
  delay(1000);

  uint8_t uidvalue[2];
  const char deveui_suffix[4];
  serialDebug.println("-- SET DEVEUI");
  uidvalue[0] = eeprom_read_byte(0);
  uidvalue[1] = eeprom_read_byte(1);
  sprintf(deveui_suffix, "%02x%02x", uidvalue[0],  uidvalue[1]);
  serialDebug.println(deveui_suffix);

  String cmd = "";
  cmd += "mac set deveui 1234567890AB";
  cmd += deveui_suffix;
  cmd.toCharArray(cmd_buffer, 256);
  sendCmd(cmd_buffer);
  delay(1000);

  sendCmd("mac get deveui");
  delay(1000);

  serialDebug.println("-- JOIN");
  /* Use our custom join with custom devEUI */
  joinOtaa(appEui, appKey);
  
  serialDebug.println("-- STATUS");
  ttn.showStatus();
}

// the loop routine runs over and over again forever:
void loop()
{
  /* Get sensors value and forge LPP message */
  if (serialDebug.available() > 0) {
    /* Get the number of data that will be received */
    nb_data = serialDebug.read();
    nb_acc += 1;
    Serial.print(nb_acc);
    delay(1000);

    /* Get and send the data to the LoRa server */
    for (int i = 0; i < nb_data; i++)
    {
      serialDebug.readBytes(line, BUFSIZE);
      serialDebug.println(line);

      // Get the information stored in the received line
      token = strtok(line, " ");
      no_zigduino = atoi(token);

      token = strtok(NULL, " ");
      t = atof(token);

      token = strtok(NULL, " ");
      h = atof(token); 

      token = strtok(NULL, " ");
      sound = atoi(token);
      
      token = strtok(NULL, " ");
      light = atoi(token);      
      
      token = strtok(NULL, " ");
      pir = atoi(token);

      beg_channel = getChannel(no_zigduino);
      
      if(beg_channel == -1)
      {
        serialDebug.println("getChannel Failed");
        break;
      }

      // Encode data
      lpp.reset();
      lpp.addTemperature(beg_channel, t);
      lpp.addRelativeHumidity(beg_channel + 1, h);
      lpp.addLuminosity(beg_channel + 2, light);
      lpp.addAnalogInput(beg_channel + 3, sound);
      lpp.addDigitalInput(beg_channel + 4, pir);

      // Send it off
      ttn.sendBytes(lpp.getBuffer(), lpp.getSize());
      delay(10000);
      
      memset(line, 0, BUFSIZE);
      nb_acc += 1;
      serialDebug.print(nb_acc);      
    }
  }
  else
  {
    // Wait 10s
    delay(10000);  
  }

  nb_acc = 0;
}