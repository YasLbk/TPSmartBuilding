
void read_temp_hum()
{
  serialDebug.println("Start reading DHT");
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  dht.temperature().getEvent(&event_temp);
  dht.humidity().getEvent(&event_hum);

  t = event_temp.temperature;
  h = event_hum.relative_humidity;

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h))
  {
    serialDebug.println("Failed to read from DHT");
    serialDebug.print("hum=");
    serialDebug.print(h);
    serialDebug.print(";temp=");
    serialDebug.print(t);
    serialDebug.println(t);
  }
  else
  {
    serialDebug.print("hum=");
    serialDebug.print(h);
    serialDebug.print(";temp=");
    serialDebug.print(t);
  }
}

boolean isPeopleDetected()
{
  int sensorValue = digitalRead(SENSOR_PIR);
  if (sensorValue == HIGH) //if the sensor value is HIGH?
  {
    return true;//yes,return true
  }
  else
  {
    return false;//no,return false
  }
}

boolean read_pir()
{
  if (isPeopleDetected()) //if it detects the moving people?
    pir = 1;
  else
    pir = 0;
  serialDebug.print("pir=");
  serialDebug.print(pir);
}

void read_light()
{
  light = analogRead(SENSOR_LIGHT);
  serialDebug.print("light=");
  serialDebug.print(light);
}

void read_sound()
{
  sound = analogRead(SENSOR_SOUND);
  serialDebug.print("sound=");
  serialDebug.print(sound);
}

void update_sensors()
{
  serialDebug.print("[IBAT];");
  read_temp_hum();
  serialDebug.print(";");
  read_light();
  serialDebug.print(";");
  read_sound();
  serialDebug.print(";");
  read_pir();
  serialDebug.println();
}