//Diese Datei beinhaltet alle Funktionen, die etwas mit auslesen der Sensoren etc. zu tun haben
void checkTemperature() {   //Diese Funktion prüft ob die gemessene Lufttemperatur den Schwellwert über- oder unterschreitet und schaltet dann das Relais
  // Relais an oder ausschalten für den Fall, dass weder das Überschussheizen noch die Nachtabsenkung aktiv ist
  if (nightReductionBool == false && heaterExcessBool == false) {
    if (temperature > setpoint / 10.0 + (hysteresisOffx10 / 10)) {
      stateRelais = false;
    } else if (temperature < setpoint / 10.0 - (hysteresisOnx10 / 10)) {
      stateRelais = true;
    }
  // Relais an oder ausschalten für den Fall, dass die Nachtabsenkung aktiv ist  
  } else if (nightReductionBool == true && heaterExcessBool == false) {
    if (temperature > (setpoint - nightReductionx10) / 10.0 + (hysteresisOffx10 / 10)) {
      stateRelais = false;
    } else if (temperature < (setpoint - nightReductionx10) / 10.0 - (hysteresisOnx10 / 10)) {
      stateRelais = true;
    }
  // Relais an oder ausschalten für den Fall, dass das Überschussheizen aktiv ist   
  } else if (heaterExcessBool == true && nightReductionBool == false) {
    if (temperature > (setpoint + heaterExcessTempx10) / 10.0 + (hysteresisOffx10 / 10)) {
      stateRelais = false;
    } else if (temperature < (setpoint + heaterExcessTempx10) / 10.0 - (hysteresisOnx10 / 10)) {
      stateRelais = true;
    }
  //Überschussheizen und Nachtabsenkung darf nicht gleichzeitig aktiv sein --> Fehlermeldung auf seriellen Port
  } else if (heaterExcessBool == true && nightReductionBool == true) {
    Serial.println("Fehler: heater_excess_bool und night_reduction_bool darf nicht gleichzeitig true sein!");
  }

  gpio_hold_dis((gpio_num_t)GPIO_NUM_17);  //Deaktiviert die Funktion, dass der Relais-Pin seinen Status beim Schlafen behält --> nötig um Ausgang schreiben zu können
  digitalWrite(RelaisPin, stateRelais);    //Setzen des Relais-Ausgangs entsprechend oben festgelegtem Status
  gpio_hold_en((gpio_num_t)GPIO_NUM_17);   //Aktiviert die Funktion, dass der Relais-Pin seinen Status beim Schlafen behält
}

void getTemperatures() {   //Diese Funtkion liest alle Werte von den Sensoren aus
  //Alle Offsets werden x10 + 120 übertragen. So können Werte von -12 °C bis +12 °C in einem Byte im EEPROM gespeichert werden
  int status = DHT.read();                                           //Werte vom DHT20 lesen
  temperature = DHT.getTemperature() + (offsetTempDHT20x10-120)/10;  //Temperatur vom DHT20 holen
  humidity = DHT.getHumidity() + (offsetHumidityx10-120)/10;         //Luftfeuchtigkeit vom DHT20 holen

  sensors.requestTemperatures();                                                     //Werte vom DS18B20 lesen
  temperatureDS18B20 = sensors.getTempCByIndex(0) + (offsetTempDS18B20x10-120)/10;   //Temperatur vom DS18B20 holen

  int x = analogRead(Floor_Temp_Pin);                                                                                                   //Spannung vom 10k NTC Bodentemperatur auslesen
  temperature_floor = -7 * pow(10, -10) * pow(x, 3) + 7 * pow(10, -6) * pow(x, 2) - 0.0379 * x + 96.519 + (offsetTempFloorx10-120)/10;  //Umrechnen in Temperatur

  //                                    Sonnensymbol      Schneesymbol
  display.temperature_is(temperature, heaterExcessBool, nightReductionBool);                     //Ist-Temperatur anzeigen
                            //Schlosssymbol
  display.setpoint(setpoint, childLockBool);                                                     //Soll-Temperatur anzeigen

  checkTemperature();  //Checken ob Relais geschalten werden muss
}
