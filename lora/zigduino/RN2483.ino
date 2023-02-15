#define PIN_SERIAL0_TX 1

/*
   func : RN2483_WAKEUP
   syno : Reset the RN2483 module
   args : nothing
   retu : nothing (void)
*/
void RN2483_WAKEUP()
{
  serialDebug.println("-- RN2483_WAKEUP");
  serialLora.end();
  pinMode(PIN_SERIAL0_TX, OUTPUT);
  // Send a break to the RN2483
  digitalWrite(PIN_SERIAL0_TX, LOW);
  delay(5);
  digitalWrite(PIN_SERIAL0_TX, HIGH);
  serialLora.begin(57600);
  // Send magic character for autobaud.
  serialLora.write(0x55);
}

/*
   func : RN2483_SLEEP
   syno : Sleep the RN2483 module for 24 hours
   args : nothing
   retu : nothing (void)
*/
void RN2483_SLEEP()
{
  serialDebug.println("-- RN2483_SLEEP");
  // ttn.sleep(1000);
  // Bug with ttn.sleep and Zigduino
  // We use raw modem command instead
  serialLora.write("sys sleep 30000");
}


void sendCmd( char *cmd)
{
  serialDebug.write(cmd);
  serialDebug.write("\r\n");
  serialLora.write(cmd);
  serialLora.write("\r\n");
  while (!serialLora.available() ) {
    delay(100);
  }
  while (serialLora.available())
    serialLora.read();
}

void waitForResponse()
{
  while (!serialLora.available() ) {
    delay(100);
  }
  while (serialLora.available())
    serialLora.read();
}

void joinOtaa(const char *appeui, const char *appkey)
{
  String cmd = "";
  if (strlen(appeui) != 16 || strlen(appkey) != 32)
  {
    serialDebug.println("ERROR KEY");
    return false;
  }

  cmd = "mac set appeui ";
  cmd += appeui;
  cmd.toCharArray(cmd_buffer, 256);
  sendCmd(cmd_buffer);

  cmd = "mac set appkey ";
  cmd += appkey;
  cmd.toCharArray(cmd_buffer, 256);
  sendCmd(cmd_buffer);

  sendCmd("mac set adr on");
  sendCmd("mac set dr 5");
  sendCmd("mac save");
  sendCmd("mac join otaa");
}