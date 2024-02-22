//Datei die alle relevanten Funktionen für das Multi-Tasking bündelt
#include <Arduino.h>

//"loop"-Funktion des erzeugten Tasks
void TaskConnectWiFI( void * pvParameters ) {
  for (;;) {
    delay(200);
    client.loop(); //ständiges abrufen von Nachrichten via MQTT
    publish_json();//Daten via MQTT versenden

    //Prüfen ob Daten versandt und empfangen wurden und ob die Hintergrundbeleuchtung aus ist
    if (messageSent && messageReceived && !backlight_LED) { 
      WiFi.disconnect(true);        //WLAN trennen
      readyForSuspension = true;    //Bool setzen, dass Task gestoppt werden kann --> wird in checkSuspendTask() überprüft
    }
  }
}

//Funktion die prüft, ob Task gestoppt werden kann
bool checkSuspendTask(){
    if (readyForSuspension) {     
    readyForSuspension = false;     //Bool ready for suspension zurücksetzen
    messageReceived = false;        //Bool message received zurücksetzen
    taskSuspended = true;          //Bool task suspended setzen
    return true;
  }else{
    wifiQuality = wifi_signal_quality(); //steht hier, dass es nach dem Trennen des WLANs nicht mehr aufgerufen wird, da es ansonsten auf null springt. 
                                          //Das entspräche zwar der Wahrheit, aber das Display soll lieber immer den letzten Wert bei funtkionierendem WiFi anzeigen, 
                                          //auch wenn es im Schlafmodus ist.
    return false;
  }
}
