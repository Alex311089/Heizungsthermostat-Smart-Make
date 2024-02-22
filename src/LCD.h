//PCF8576C pins
//A0->Vdd A1->Vss A2->Vss (Gnd) => device address 001
//SA0->Vdd  => I2C bus slave address B0111001 (sonst Kollision mit DHT20)
//OSC->Vss (Gnd) => internal clock
//BP0,BP1,BP2,BP3->LCD => vier backplanes, multiplex drive mode 1:4, bias 1/3

//weitere Infos:
///https://bluetechs.wordpress.com/zothers/x/arduino-code/
//https://github.com/nbagaev/PCF8576/blob/master/PCF8576.ino
//https://forum.arduino.cc/t/i2c-display-pcf8576dh-driver/948936

class LCD {
private:
#define busSlaveAddress B0111001  //I2C bus slave address  (011100) (SA0)
                                    //                                                          (011100)  (1)
#define modeSet B11001000          //mode set (command)(fixed value)(power-saving mode)(enable)(bias)(multiplex drive mode)
                                    //                                        (1)        (10)             (0)          (1)    (0)         (00)
#define deviceSelect B11100001     //device address (command)(fixed value)(A2 A1 A0)
                                    //                                                   (1)      (1100)      (001)

// Pointer auf alle 7 Bytes die zum anzeigen von Ziffern auf dem Display gebraucht werden.
#define pointerUpperLeft 7          //Ziffer oben links
#define pointerUpperMiddle 5        //Ziffer oben Mitte
#define pointerUpperRight 3         //Ziffer oben rechts
#define pointerLowerLeft 17         //Ziffer unten links
#define pointerLowerMiddleLeft 15   //Ziffer unten Mitte links
#define pointerLowerMiddleRight 11  //Ziffer unten Mitte rechts
#define pointerLowerRight 9         //unten rechts

  int numbers[10] = { B01111101, B01100000, B00111110, B01111010, B01100011, B01011011, B01011111, B01110000, B01111111, B01111011 };
  //                    null        eins      zwei       drei        vier       fünf      sechs      sieben      acht      neun

  // Private Funktion die ein Byte für eine Ziffer an das Display überträgt.
  void displayNumber(int pointer, int number, bool additionalSymbol) {
    int data = numbers[number];

    if (pointer == pointerUpperRight || pointer == pointerLowerRight) {
      data = data | B10000000;                        // Fügt dem Byte für Ziffer oben rechts bzw. unten rechts das erste Bit für Segment 13 (Dezimalpunkt, Thermometersymbol etc.) hinzu bzw. 37 (°C)
    }

    if (pointer == pointerUpperLeft && additionalSymbol) {  //Sonnensymbol
      data = data | B10000000;                        // Fügt dem Byte für Ziffer oben links das erste Bit für Segment 29 (Sonnensymbol) hinzu
    }

    if (pointer == pointerUpperMiddle && additionalSymbol) {  //Schneflocke
      data = data | B10000000;                        // Fügt dem Byte für Ziffer oben links das erste Bit für Segment 21 (Schneflocke) hinzu
    }

    if (pointer == pointerLowerLeft && additionalSymbol) {  //Schloss Symbol
      data = data | B10000000;                        // Fügt dem Byte für Ziffer oben links das erste Bit für Segment 69 (Schloss Symbol) hinzu
    }

    display_byte(pointer, data);                      //Überträgt das oben gebaute Byte an den Displaytreiber
  }

  //Funktion, die ein Byte an den Displaytreiber überträgt
  void display_byte(int pointer, int data) {
    Wire.beginTransmission(busSlaveAddress);
    Wire.write(modeSet);
    Wire.write(deviceSelect);
    Wire.write(pointer);
    Wire.write(data);
    Wire.endTransmission();
  }

public:

  // Öffentliche Funktion die alle Display-Segmente an und ausschalten kann (Funktionstest o.Ä.)
  void allSegmentsOn(bool on) {
    uint8_t data;
    uint8_t dataPointer = B00000000;
    Wire.beginTransmission(busSlaveAddress);
    Wire.write(modeSet);
    Wire.write(deviceSelect);
    Wire.write(dataPointer);

    if (on)
      data = B11111111;
    else
      data = B00000000;

    //Alle 20 Bytes schreiben, die alle Segmente des Displays abdecken
    for (uint8_t i = 0; i < 20; i++) {
      Wire.write(data);
      Wire.write(data);
    }
    Wire.endTransmission();
  }


  // Öffentliche Funtkion die die Wire-Kommunikation startet
  void initLCD() {
    Wire.begin();
  }


  // Öffentliche Funtkion die die ist-Temperatur auf den oberen drei Segmenten anzeigt.
  void temperature_is(float temperature, bool sunsymbol, bool snowsymbol) {
    int temperaturex10 = temperature * 10;
    int onesDigitIs = temperaturex10 % 10;
    int tensDigitIs = (temperaturex10 / 10) % 10;
    int hundredsDigitIs = (temperaturex10 / 100) % 10;

    displayNumber(pointerUpperLeft, hundredsDigitIs, sunsymbol);
    displayNumber(pointerUpperMiddle, tensDigitIs, snowsymbol);
    displayNumber(pointerUpperRight, onesDigitIs, true);
  }


  // Öffentliche Funtkion die die soll-Temperatur auf den unteren vier Segmenten anzeigt.
  void setpoint(int setpoint, bool lockSymbol) {
    int setpointx10 = setpoint;
    int onesDigitSetpoint = setpointx10 % 10;
    int tensDigitSetpoint = (setpointx10 / 10) % 10;
    int hundredsDigitSetpoint = (setpointx10 / 100) % 10;

    int decimalPoint = B10000000;
    int pointerDecimalPoint = 0;

    int airsign = B00000001;
    int pointerAirsign = 1;

    displayNumber(pointerLowerLeft, hundredsDigitSetpoint, lockSymbol);
    displayNumber(pointerLowerMiddleLeft, tensDigitSetpoint, false);
    displayNumber(pointerLowerMiddleRight, onesDigitSetpoint, false);
    displayNumber(pointerLowerRight, 0, false);
    display_byte(pointerDecimalPoint, decimalPoint);
    display_byte(pointerAirsign, airsign);
  }

  //Funktion die die WiFi Signalstärke in vier Stufen ausgibt (0-3)
  //                        0-3
  void displayWifi(int signalStrength, bool valve) {
    int pointerWifi = 13;
    int Data = B00000000;

    switch (signalStrength) {
      case 0:
        Data = B00000000;
        break;
      case 1:
        Data = B00100000;
        break;
      case 2:
        Data = B01100000;
        break;
      case 3:
        Data = B11100000;
        break;
    }
    if (valve) {
      Data = Data | B00000100;  //Fügt data Bit 6 für das Ventil-Symbol hinzu
    }
    display_byte(pointerWifi, Data);
  }
};


