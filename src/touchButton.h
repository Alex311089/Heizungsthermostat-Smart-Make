void probe_touch(); //Deklaration der Funktion

//Die folgenden Funktionen werden aufgerufen, wenn das Display eine Berührung registriert hat. touchedTx, speichert, welcher Knopf es war
void IRAM_ATTR T0wasActivated() {
  touchedTx = 0;
}
void IRAM_ATTR T2wasActivated() {
  touchedTx = 2;
}
void IRAM_ATTR T3wasActivated() {
  touchedTx = 3;
}
void IRAM_ATTR T4wasActivated() {
  touchedTx = 4;
}
void IRAM_ATTR T5wasActivated() {
  touchedTx = 5;
}

//Funktion zum Entprellen der Eingabe bzw. um mehrfach-Eingaben zu vermeiden
bool touchDelayComp(unsigned long lastTouch) {
  if (millis() - lastTouch < touchDelay) return false;
  return true;
}

//Funktion die überprüft welcher Knopf gedrückt wurde
void probe_touch() {
  if (touchedTx != 99) {
    switch (touchedTx) {  //Prüft auf die verschiedenen möglichen Knöpfe
      case 0: //Ein nicht genutzter Knopf --> es wird nur die Hintergrundbeleuchtung angeschalten
        if (touchDelayComp(sinceLastTouch) && !childLockBool) {   //Prüfen auf Entprellung und ausgeschaltene Kindersicherung
          backlight_LED = true;           //Hintergrundbeleuchtung an
          digitalWrite(LEDPin, HIGH);     //Hintergrundbeleuchtung an
          sinceLastTouch = millis();      //Zeitmarke für Entprellung setzen
          wakeSince = millis();           //Zeitmarke für Überprüfung aktive Zeit setzen
        }
      case 2: //Temperatur erhöhen Knopf --> es wird die Hintergrundbeleuchtung angeschalten und die Solltemperatur erhöht
        if (touchDelayComp(sinceLastTouch) && !childLockBool) {   //Prüfen auf Entprellung und ausgeschaltene Kindersicherung
          backlight_LED = true;                         //Hintergrundbeleuchtung an
          digitalWrite(LEDPin, HIGH);                   //Hintergrundbeleuchtung an
          if ((setpoint <= 250) && backlight_LED) {     //nur erhöhen, wenn 250 nicht überschritten ist, da EEPROM bei über 255 überläuft!
            setpoint = setpoint + 5;                    //Solltemperatur erhöhen
            display.setpoint(setpoint, childLockBool);  //Gibt aktuellen setpoint auf dem Display aus (verhindert zu viel Delay)
            EEPROM.write(EEPROMSetpoint, setpoint);    //Neuen Sollwert in EEPROM schreiben
            EEPROM.commit();                            //EEPROM comitten
          }
          sinceLastTouch = millis();                    //Zeitmarke für Entprellung setzen
          wakeSince = millis();                         //Zeitmarke für Überprüfung aktive Zeit setzen
        }
      case 3: //Temperatur erniedrigen Knopf --> es wird die Hintergrundbeleuchtung angeschalten und die Solltemperatur erniedrigt
        if (touchDelayComp(sinceLastTouch) && !childLockBool) {   //Prüfen auf Entprellung und ausgeschaltene Kindersicherung
          backlight_LED = true;                         //Hintergrundbeleuchtung an
          digitalWrite(LEDPin, HIGH);                   //Hintergrundbeleuchtung an
          if ((setpoint >= 5) && backlight_LED) {       //nur erhöhen, wenn 5 nicht unterschritten ist, da EEPROM unter 0 überläuft!    
            setpoint = setpoint - 5;                    //Solltemperatur erniedrigen
            display.setpoint(setpoint, childLockBool);  //Gibt aktuellen setpoint auf dem Display aus (verhindert zu viel Delay)
            EEPROM.write(EEPROMSetpoint, setpoint);    //Neuen Sollwert in EEPROM schreiben
            EEPROM.commit();                            //EEPROM comitten
          }
          sinceLastTouch = millis();                    //Zeitmarke für Entprellung setzen
          wakeSince = millis();                         //Zeitmarke für Entprellung setzen
        }
      case 4: //Ein nicht genutzter Knopf --> es wird nur die Hintergrundbeleuchtung angeschalten
        if (touchDelayComp(sinceLastTouch) && !childLockBool) {   //Prüfen auf Entprellung und ausgeschaltene Kindersicherung
          backlight_LED = true;                         //Hintergrundbeleuchtung an
          digitalWrite(LEDPin, HIGH);                   //Hintergrundbeleuchtung an
          sinceLastTouch = millis();                    //Zeitmarke für Entprellung setzen
          wakeSince = millis();                         //Zeitmarke für Entprellung setzen
        }
      case 5: //Ein nicht genutzter Knopf --> es wird nur die Hintergrundbeleuchtung angeschalten
        if (touchDelayComp(sinceLastTouch) && !childLockBool) {   //Prüfen auf Entprellung und ausgeschaltene Kindersicherung
          backlight_LED = true;                         //Hintergrundbeleuchtung an
          digitalWrite(LEDPin, HIGH);                   //Hintergrundbeleuchtung an
          sinceLastTouch = millis();                    //Zeitmarke für Entprellung setzen
          wakeSince = millis();                         //Zeitmarke für Entprellung setzen
        }
    }
    touchedTx = 99; //touchedTx auf ansonsten ungenutzten Wert setzen
  }
}

//Funktion die überprüft, ob die Hintergrundbeleuchtung an- oder ausgeschaltet werden muss
void check_backlight() {
  if (backlight_LED) {
    digitalWrite(LEDPin, HIGH);

    if (millis() - sinceLastTouch > LEDOnDuration) {  //hier ist sinceLastTouch besser geeignet als ein eigener Timer, da er im Touch case ganz oben gesetzt werden müsste und genau das gleiche macht. Außerdem resettet jeder neue Touch den Timer
      digitalWrite(LEDPin, LOW);
      backlight_LED = false;
    }
  }
}
