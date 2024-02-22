#include "includes.h"           //eigene Datei includes.h einbinden, die alle notwendigen includes enthält

void setup() {
  Wire.begin();                  // Muss hier steehen so dass die Wire-Kommunikation vor display.allSegmentsOn(false); initialisiert ist
  display.allSegmentsOn(false);  //Alle Segmente des Displays aus
  pinMode(LEDPin, OUTPUT);      //LED Pin als Output definieren
  pinMode(RelaisPin, OUTPUT);   //Relais Pin als Output definieren
  digitalWrite(LEDPin, LOW);    //LED ausschalten
  DHT.begin();                   //Beginne Kommunikation mit DHT20
  Serial.begin(115200);          //Beginne serielle Kommunikation
  display.initLCD();             //Initialisiere LCD-Display
  EEPROM.begin(EEPROMSize);     //Initialisiere EEPROM
  sensors.begin();               //Initialisiere DS18B20


  //Interrupts an Touch-Pins anhängen
  touchAttachInterrupt(T0, T0wasActivated, thresholdTouch);
  touchAttachInterrupt(T2, T2wasActivated, thresholdTouch);
  touchAttachInterrupt(T3, T3wasActivated, thresholdTouch);
  touchAttachInterrupt(T4, T4wasActivated, thresholdTouch);
  touchAttachInterrupt(T5, T5wasActivated, thresholdTouch);

  esp_sleep_enable_touchpad_wakeup();                  //Touch-Eingabe als Aufwachquwelle aus dem Schlaf aktivieren
  esp_sleep_enable_timer_wakeup(periodWakeup * 1000);  //Timer um sleep zu beenden ; Zeit in µs --> daher ms * 1000

  //Alle Werte nach reboot aus dem EEPROM laden
  setpoint = EEPROM.read(0);
  nightReductionBool = EEPROM.read(EEPROMNightReductionBool);
  nightReductionx10 = EEPROM.read(EEPROMNightReductionx10);
  heaterExcessBool = EEPROM.read(EEPROMHeaterExcessBool);
  heaterExcessTempx10 = EEPROM.read(EEPROMHeaterExcessTempx10);
  childLockBool = EEPROM.read(EEPROMChildLockBool);
  hysteresisOnx10 = EEPROM.read(EEPROMHysteresisOnx10);
  hysteresisOffx10 = EEPROM.read(EEPROMHysteresisOffx10);
  offsetTempDHT20x10 = EEPROM.read(EEPROMOffsetTempDHT20x10);
  offsetTempDS18B20x10 = EEPROM.read(EEPROMOffsetTempDS18B20x10);
  offsetTempFloorx10 = EEPROM.read(EEPROMOffsetTempFloorx10);
  offsetHumidityx10 = EEPROM.read(EEPROMOffsetHumidityx10);

  xTaskCreate(TaskConnectWiFI, "TaskConnectWiFi", 3000,             NULL,             0,       &Task1);  //WiFi-Task erzeugen
  //           Task function,    name of task,  Stack size, parameter of the task, priority, Task handle
  //priority 0 ist äußerst wichtig, da ansonsten die Betriebssystem-Tasks keine Ausführungszeit erhalten, es kommt zum Error wegen Watchdog-Triggerung

  taskIsRunning = true;  //Setze Bool task_running auf true
}

void loop() {
  probe_touch();        //überprüft ob das Touch-Panel berührt wurde und entsprechend weitere Funtkionen aufruft
  check_backlight();    //überprüft ob die Zeitdauer, die die LED-Hintergrundbeleuchtung an bleiben soll abgelaufen ist und schaltet die LED ggf. ab
  getTemperatures();    //liest alle Temperaturen + Luftfeuchte von den Sensoren aus
  display.displayWifi(wifiQuality, stateRelais); //zeigt die WiFi-Qualität und den Status des Relais auf dem Display an (die drei WiFi-Stufen und das Relais-Symbol liegt in einem Byte)
  
  if (esp_get_minimum_free_heap_size() < 5000) {  //Resetted den ESP32 wenn der Heap zuneige geht
    ESP.restart();
  }

  if (checkSuspendTask()) {   //Stoppt den laufenden WiFi Task (nachdem er selbst WiFi getrennt hat)
    vTaskSuspend(Task1);      //Task stoppen
    taskIsRunning = false;    //Bool auf Task läuft nicht, also false setzen
    esp_light_sleep_start();  //Schlafen starten
  }

  if (!taskIsRunning) {   //Startet den WiFi Task, wenn er nicht läuft --> also immer nach dem Aufwachen - ganze if wird nur einmal nach dem Aufwachen ausgeführt.
    vTaskResume(Task1);   //Setzt den WiFi Task fort, wenn er nicht läuft --> also immer nach dem Aufwachen Wichtig: Es muss vTaskResume() verwendet werden, xTaskCreatePinnedToCore() bringt den Heap zum überlaufen, da viele Tasks erzeugt werden.
    taskIsRunning = true; //bool auf Task läuft, also true setzen
    wakeSince = millis(); //Zeit seit der der ESP32 wach ist in die Variable wakeSince schreiben
  }

  if (millis() - wakeSince > maxUptime) {   //Prüfen ob ESP32 länger wach ist als erlaubt (maxUptime) --> verhindert vor allem deadlocks wenn Verbindungsaufbau fehlschlägt
    digitalWrite(LEDPin, LOW);              //LED-Hintergrundbeleuchtung ausschalten
    WiFi.disconnect(true);                  //WLAN trennen
    vTaskSuspend(Task1);                    //Task stoppen  
    taskIsRunning = false;                  //Bool auf Task läuft nicht, also false setzen
    esp_light_sleep_start();                //Schlafen starten
  }
}
