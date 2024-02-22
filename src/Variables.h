//In dieser Datei stehen alle Variablendefinitionen

//**********MQTT Topic und Client Name**********
const char MQTTTopicReceive[] = "Fussbodenthermostate/Development/control";
const char MQTTTopicSend[] = "Fussbodenthermostate/Development/data";
const char clientName[] = "Fussbodenthermostat_Development";

//**********Hardware-Pins**********
const int LEDPin = 16;                      //LED an IO16
const int RelaisPin = 17;                   //Relais an IO17
const int Floor_Temp_Pin = 34;              //Pin Temperaturfühler Boden
const int oneWireBus = 27;                  //GPIO where the DS18B20 is connected to

//**********Variablen für Zeitablaufsteuerung**********
long periodWakeup = 5*60*1000;              //Zeit, nach der der ESP32 aus dem Schlaf erwacht
long LEDOnDuration = 10000;                 //Anschaltdauer für Hintergrundbeleuchtung
long long sinceLastTouch = 0;               //Timer für Entprellung Touch Buttons
long touchDelay = 350; //ms                 //Entprelldauer für Touch Buttons
long long wakeSince = 0;                    //Timer zur Überwachung wie lange der ESP schon wach ist
long maxUptime = 15000;                     //Maximale Zeit, die der ESP wach sein darf

//**********Variablen für Touch-Steuerung**********
int touchedTx = 99;                         //gibt an welcher Touch Button gedrückt wurde
const uint8_t thresholdTouch = 1;           //Schwellwert für Touch Buttons

//***********Variablen für WiFi-Empfangsqualität***********
int wifiPercent;                           //Prozentwert WiFi-Empfang
int wifiQuality = 0;                       //WiFi-Empfang in Stufen 0-3 für Display

//***********Bools für Task-Managing***********
bool messageSent = false;                  //Bool Nachricht versand   
bool messageReceived = false;              //Bool Nachricht empfangen
bool readyForSuspension = false;           //Bool Task kann beendet werden
bool taskSuspended = false;                //Bool Task ist aktuell beendet
bool taskIsRunning = false;                //Bool Task läuft gerade

//**********Variablen für Temperaturmessung und -steuerung***********
float hysteresisOffx10 = 5;                 //Hysterese an
float hysteresisOnx10 = 5;                  //Hysterese aus
float setpoint = 0;                         //Sollwert des Thermostaten
float temperature;                          //Temperatur DHT20
float offsetTempDHT20x10;                   //Offset Temperatur DHT20
float temperature_floor;                    //Temperatur Boden
float offsetTempFloorx10;                   //Offset Temperatur Boden
float temperatureDS18B20;                   //Temperatur DS18B20
float offsetTempDS18B20x10;                 //Offset Temperatur DS18B20
float humidity;                             //Luftfeuchtigkeit DS18B20
float offsetHumidityx10;                    //Offset Luftfeuchtigkeit DS18B20
float nightReductionx10;                    //Höhe der Nachtabsenkung *10 (wegen speichern in EEPROM)
float heaterExcessTempx10;                  //Höhe der Temperaturerhöhung *10 bei PV-Überschuss

//***********Bools für Temperaturregelung und Display-Steuerung***********
bool stateRelais;                          //Status des Relais
bool nightReductionBool;                   //Status ob Nachtabsenkung aktiv
bool backlight_LED=false;                  //Status ob Hintergrundbeleuchtung an ist
bool heaterExcessBool = false;             //Status ob Temperatur wegen PV-Überschuss erhöht wird
bool childLockBool = false;                //Status ob Kindersicherung an

//**********EEPROM-Adressen**********
int EEPROMSetpoint = 0;                     //Adresse Sollwert
int EEPROMNightReductionBool = 1;           //Adresse Bool Nachtabsenkung
int EEPROMNightReductionx10 = 2;            //Adresse Temperatur Delta Nachtabsenkung 
int EEPROMHeaterExcessBool = 3;             //Adresse Bool Überschussheizen
int EEPROMHeaterExcessTempx10 = 4;          //Adresse Temperatur Delta Überschussheizen
int EEPROMChildLockBool = 5;                //Adresse Bool Kindersicherung
int EEPROMHysteresisOnx10 = 6;              //Adresse Hysterese an
int EEPROMHysteresisOffx10 = 7;             //Adresse Hysterese aus
int EEPROMOffsetTempDHT20x10 = 8;           //Adresse Offset Sensortemperatur DHT20
int EEPROMOffsetTempDS18B20x10 = 9;         //Adresse Offset Sensortemperatur DS18B20
int EEPROMOffsetHumidityx10 = 10;           //Adresse Offset Luftfeuchte
int EEPROMOffsetTempFloorx10 = 11;          //Adresse Offset Sensor Bodentemperatur
