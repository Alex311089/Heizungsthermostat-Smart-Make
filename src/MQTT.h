//In dieser Datei stehen alle Funktionen rund um MQTT und Datentransfer
EspMQTTClient client(   //Mit WLAN und MQTT Broker verbinden
    "SSID",             //SSID des WLANs
    "Passwort",         //Passwort des WLANs
    "xxx.xxx.xxx.xxx",  //MQTT Broker IP Adresse
    "",                 //MQTT Username --> kann ausgelassen, wenn nicht benötigt / vergeben
    "",                 //MQTT Passwort --> kann ausgelassen, wenn nicht benötigt / vergeben
    clientName          //ClientName zur Identifizierung des Client
                        //MQTT-Port --> kann im Falle von 1883 weggelassen werden
);

//Funktion um Werte in EEPROM zu schreiben; schreibt nur dann, wenn zu schreibende Daten vom Inhalt abweichen --> EEPROM hat nur eine begrenzte Anzahl an Schreibzugriffen
void write_EEPROM(int adress, int Data)
{
  int EEPROM_content = EEPROM.read(adress);
  if (EEPROM_content != Data)
  {
    EEPROM.write(adress, Data);
    EEPROM.commit();
  }
}

//Funktion die nach erfolgreichem Verbindungsaufbau mit WLAN und MQTT aufgerufen wird
void onConnectionEstablished()
{
  //Topic für zu empfangende Daten abonnieren
  client.subscribe(MQTTTopicReceive, [](const String &payload)
                   {
    if (payload.indexOf("nrb")) {  //Prüfen ob richtige Daten empfangen wurden (enthalten "nrb"), dann Bool messageReceived true setzen
      messageReceived = true;
    }
    DynamicJsonDocument doc(1024);  //JSON Dokument ausreichender Länge anlegen
    deserializeJson(doc, payload);  //empfangene Payload in Dokument schreiben
    //Alle empfangenen Daten in ihre Variablen schreiben
    nightReductionBool = doc["nrb"];
    nightReductionx10 = doc["nrx10"];
    heaterExcessBool = doc["heb"];
    heaterExcessTempx10 = doc["hex10"];
    childLockBool = doc["clb"];
    hysteresisOnx10 = doc["hon"];
    hysteresisOffx10 = doc["hoff"];
    offsetTempDHT20x10 = doc["offDHT"];
    offsetTempDS18B20x10 = doc["offDS"];
    offsetTempFloorx10 = doc["offFl"];
    offsetHumidityx10 = doc["offHu"];

    //Alle empfangenen Daten in den EEPROM schreiben
    write_EEPROM(EEPROMNightReductionBool, nightReductionBool);
    write_EEPROM(EEPROMNightReductionx10, nightReductionx10);
    write_EEPROM(EEPROMHeaterExcessBool, heaterExcessBool);
    write_EEPROM(EEPROMHeaterExcessTempx10, heaterExcessTempx10);
    write_EEPROM(EEPROMChildLockBool, childLockBool);
    write_EEPROM(EEPROMHysteresisOnx10, hysteresisOnx10);
    write_EEPROM(EEPROMHysteresisOffx10, hysteresisOffx10);
    write_EEPROM(EEPROMOffsetTempDHT20x10, offsetTempDHT20x10);
    write_EEPROM(EEPROMOffsetTempDS18B20x10, offsetTempDS18B20x10);
    write_EEPROM(EEPROMOffsetTempFloorx10, offsetTempFloorx10);
    write_EEPROM(EEPROMOffsetHumidityx10, offsetHumidityx10); });
}

//Funktion um Messwerte über MQTT zu senden
void publish_json()
{
  char JSON_char[256];          //JSON Dokument Größe definieren
  StaticJsonDocument<200> doc;  //JSON Dokument anlegen
  //Alle zu sendenden Variablen in JSON Dokument schreiben
  doc["temperature_DHT20"] = temperature;
  doc["temperature_floor"] = temperature_floor;
  doc["setpoint"] = double(setpoint) / 10;
  doc["humidity"] = humidity;
  doc["relais_state"] = stateRelais;
  doc["WiFi_RSSI"] = WiFi.RSSI();
  doc["WiFi_percent"] = min(max(2 * (WiFi.RSSI() + 100), 0), 100);
  doc["temp_DS18B20"] = temperatureDS18B20;
  doc["remaining_heap"] = esp_get_minimum_free_heap_size();

  serializeJson(doc, JSON_char);  //Dokument versandfertig machen
  messageSent = client.publish(MQTTTopicSend, JSON_char);  //Dokument an Topic für zu sendende Daten senden
}

//Funktion die WiFi Signalstärke ausliest
int wifi_signal_quality()
{
  if (WiFi.RSSI() != 0)   //Signastärke in dB auslesen
  {
    wifiPercent = min(max(2 * (WiFi.RSSI() + 100), 0), 100); //Signastärke in % umrechnen
  }
  else
  {
    wifiPercent = 0;
  }
  //Signalstärke in vier Kategorien für die Anzeige einsortieren
  switch (wifiPercent)
  {
  case 0:
    return 0;
  case 1 ... 32:
    return 1;
  case 33 ... 65:
    return 2;
  case 66 ... 100:
    return 3;
  }

}
