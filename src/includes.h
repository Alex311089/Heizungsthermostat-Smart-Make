//In dieser Datei stehen aus Überscihtlichkeitsgründen alle includes
//Die Reihenfolge ist wichtig für Sichtbarkeit der einzelnen Module etc.
#include <Wire.h>                   //Bibliothek Wire einbinden
#include <EEPROM.h>                 //Bibliothek EEPROM einbinden
#include <OneWire.h>                //Bibliothek OneWire einbinden
#include <DallasTemperature.h>      //Bibliothek DallasTemperature einbinden
#include "EspMQTTClient.h"          //Bibliothek EspMQTTClient einbinden
#include <ArduinoJson.h>            //Bibliothek ArduinoJson einbinden
#include "DHT20.h"                  //Bibliothek DHT20 einbinden
#include "Variables.h"              //Eigene Datei "Variables" einbinden; Muss vor allen anderen stehen, damit die Variablen gesehen werden
DHT20 DHT;                          //DHT20 Sensor definieren
OneWire oneWire(oneWireBus);        //oneWirewBus definieren; muss nach Variables.h stehen, damit der Pin oneWireBus gesehen wird.
DallasTemperature sensors(&oneWire);//Temperatursensor definieren
#include "LCD.h"                    //Eigene Datei "LCD" einbinden
LCD display;                        //display definieren; Muss hier stehen, damit es von "touchButton.h", "MQTT.h" und "temperatures.h" gesehen wird.
#include "touchButton.h"            //Eigene Datei "touchButton" einbinden
#include "MQTT.h"                   //Eigene Datei "MQTT" einbinden
#include "TaskManaging.h"           //Eigene Datei "TaskManaging" einbinden
TaskHandle_t Task1;                 //Task1 definieren
#include "temperatures.h"           //Eigene Datei "temperatures" einbinden
#define EEPROMSize 15               //Größe des EEPROM definieren
