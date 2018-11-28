# CampusIoT :: SODAQ ExpLoRer :: Tutoriel :fr:

Ce tutoriel a pour objectif de programmer une carte [SODAQ ExpLoRer](https://support.sodaq.com/sodaq-one/explorer/) et de l'enregistrer sur le réseau LoRaWAN de CampusIoT.

La carte [SODAQ ExpLoRer](https://support.sodaq.com/sodaq-one/explorer/) se présente comme les 2 figures ci-dessous:

![SODAQ ExpLoRer](http://support.sodaq.com/wp-content/uploads/2018/02/explorer6b.png)

![SODAQ Pinout](https://support.sodaq.com/wp-content/uploads/2017/10/explorer-pinout.png)

## Installation
Suivre l'installation du tutoriel du SODAQ ExpLoRer https://support.sodaq.com/sodaq-one/explorer/

Le support pour LoRaWAN est décrit ici : https://support.sodaq.com/sodaq-one/lorawan/

## Installer le programme sur la carte

Ouvrir un nouveau sketch et copier le sketch suivant:

```
// From https://support.sodaq.com/sodaq-one/lorawan/

#include <Sodaq_RN2483.h>

#define debugSerial SerialUSB
#define loraSerial Serial2

#define NIBBLE_TO_HEX_CHAR(i) ((i <= 9) ? ('0' + i) : ('A' - 10 + i))
#define HIGH_NIBBLE(i) ((i >> 4) & 0x0F)
#define LOW_NIBBLE(i) (i & 0x0F)

// TODO add #ifdef for OTAA/ABP in order to shrink the firmware
//Use OTAA, set to false to use ABP
bool OTAA = true;

// ABP
// USE YOUR OWN KEYS!
const uint8_t devAddr[4] =
{
    0x00, 0x00, 0x00, 0x00
};

// USE YOUR OWN KEYS!
const uint8_t appSKey[16] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// USE YOUR OWN KEYS!
const uint8_t nwkSKey[16] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// OTAA
// With using the GetHWEUI() function the HWEUI will be used
// 0cafcb0000e01234
const uint8_t DevEUI[8] =
{
 0x0c, 0xaf, 0xcb, 0x00, 0x00, 0xe0, 0x12, 0x34
};

const uint8_t AppEUI[8] =
{
  0x0c, 0xaf, 0xcb, 0x00, 0x00, 0xff, 0xff, 0xff
};

// 0400123428aed2a6abf7158809cf4f3c
const uint8_t AppKey[16] =
{
0x04, 0x00, 0x12, 0x34, 0x28, 0xae, 0xd2, 0xa6,
0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

void setup()
{
  delay(1000);

  while ((!debugSerial) && (millis() < 10000)){
    // Wait 10 seconds for debugSerial to open
  }

  debugSerial.println("Start");

  // Start streams
  debugSerial.begin(57600);
  loraSerial.begin(LoRaBee.getDefaultBaudRate());

  LoRaBee.setDiag(debugSerial); // to use debug remove //DEBUG inside library
  LoRaBee.init(loraSerial, LORA_RESET);

  //Use the Hardware EUI
  getHWEUI();

  // Print the Hardware EUI
  debugSerial.print("LoRa HWEUI: ");
  for (uint8_t i = 0; i < sizeof(DevEUI); i++) {
	  debugSerial.print((char)NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(DevEUI[i])));
	  debugSerial.print((char)NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(DevEUI[i])));
  }
  debugSerial.println();  

  setupLoRa();
}

void setupLoRa(){
  if(!OTAA){
    // ABP
    setupLoRaABP();
  } else {
    //OTAA
    setupLoRaOTAA();
  }
  // Uncomment this line to for the RN2903 with the Actility Network
  // For OTAA update the DEFAULT_FSB in the library
  // LoRaBee.setFsbChannels(1);

  LoRaBee.setSpreadingFactor(9);
}

void setupLoRaABP(){  
  if (LoRaBee.initABP(loraSerial, devAddr, appSKey, nwkSKey, true))
  {
    debugSerial.println("Communication to LoRaBEE successful.");
  }
  else
  {
    debugSerial.println("Communication to LoRaBEE failed!");
  }
}

void setupLoRaOTAA(){

  if (LoRaBee.initOTA(loraSerial, DevEUI, AppEUI, AppKey, true))
  {
    debugSerial.println("Network connection successful.");
  }
  else
  {
    debugSerial.println("Network connection failed!");
  }
}

void loop()
{
  //String reading = getTemperature();
  float reading = getTemperatureFloat();
   debugSerial.println(reading);

//   switch (LoRaBee.send(1, (uint8_t*)reading.c_str(), reading.length()))
   switch (LoRaBee.send(1, (float*)reading.c_str(), reading.length()))
    {
    case NoError:
      debugSerial.println("Successful transmission.");
      break;
    case NoResponse:
      debugSerial.println("There was no response from the device.");
      break;
    case Timeout:
      debugSerial.println("Connection timed-out. Check your serial connection to the device! Sleeping for 20sec.");
      delay(20000);
      break;
    case PayloadSizeError:
      debugSerial.println("The size of the payload is greater than allowed. Transmission failed!");
      break;
    case InternalError:
      debugSerial.println("Oh No! This shouldn't happen. Something is really wrong! The program will reset the RN module.");
      setupLoRa();
      break;
    case Busy:
      debugSerial.println("The device is busy. Sleeping for 10 extra seconds.");
      delay(10000);
      break;
    case NetworkFatalError:
      debugSerial.println("There is a non-recoverable error with the network connection. The program will reset the RN module.");
      setupLoRa();
      break;
    case NotConnected:
      debugSerial.println("The device is not connected to the network. The program will reset the RN module.");
      setupLoRa();
      break;
    case NoAcknowledgment:
      debugSerial.println("There was no acknowledgment sent back!");
      break;
    default:
      break;
    }
    // Delay between readings
    // 60 000 = 1 minute
    delay(10000);
}

String getTemperature()
{
  //10mV per C, 0C is 500mV
  float mVolts = (float)analogRead(TEMP_SENSOR) * 3300.0 / 1023.0;
  float temp = (mVolts - 500.0) / 10.0;

  return String(temp);
}
// TODO for Cayenne LPP
// TODO https://github.com/myDevicesIoT/cayenne-docs/blob/master/docs/LORA.md
// TODO https://github.com/aabadie/cayenne-lpp
int16_t getTemperatureInt16()
{
  //10mV per C, 0C is 500mV
  float mVolts = (float)analogRead(TEMP_SENSOR) * 3300.0 / 1023.0;
  int16_t temp = (int16_t) (((mVolts - 500.0) / 10.0) * 100);

  return temp;
}

float getTemperatureFloat()
{
  //10mV per C, 0C is 500mV
  float mVolts = (float)analogRead(TEMP_SENSOR) * 3300.0 / 1023.0;
  float temp = ((mVolts - 500.0) / 10.0);

  return temp;
}

// TODO get ADC grove connector value
// TODO get GNSS module geolocation value on RX/TX pins. See http://forum.sodaq.com/search?q=GPS

/**
* Gets and stores the LoRa module's HWEUI/
*/
static void getHWEUI()
{
	uint8_t len = LoRaBee.getHWEUI(DevEUI, sizeof(DevEUI));
}
```
Ajouter les 2 bibliothèques SODAQ_wdt et SODAQ_RN2483 au sketch avec "Croquis > Inclure une bibliothèque > Gérer les bibliothèques" (filtrer la liste avec le mot clé "SODAQ").

TODO : Ajouter les 2 fichiers https://github.com/aabadie/cayenne-lpp dans le répertoire du sketch.

Configurer le DevEUI et l'AppKey dans le sketch.

Compiler et charger (ie flash) le sketch sur la carte.

[Plus d'info](https://github.com/Orange-OpenSource/Orange-ExpLoRer-Kit-for-LoRa)

## Enregistrer le device SODAQ Explorer
Du coté de la console https://lora.campusiot.imag.fr

Créer une application `SODAQ_EXPLORER` avec le service-profile `DEFAULT`

Ajouter un device avec `+ Add` en utilisant le `DevEUI` et l'`AppKey` (Voir)

Afficher les messages du device depuis l'onglet "`Live LoRaWAN Frame`"

En parallèle, afficher les traces du sketch dans le moniteur serie de l'IDE Arduino.

[Plus de détail sur l'enregistrement de devices](../loraserver/README.md#cr%C3%A9er-une-application)

## Décoder les frames sur le serveur

Ajouter la fonction de décodage "`Application > SODAQ_EXPLORER > Application Configuration > Payload Coded > Custom Javascript codec functions`" suivante:

```
// Decode decodes an array of bytes into an object.
//  - fPort contains the LoRaWAN fPort number
//  - bytes is an array of bytes, e.g. [225, 230, 255, 0]
function Decode(fPort, bytes) {
  return { temperature: };
}
```

## Pour aller plus loin
[Tutoriel NodeRED](../nodered/README.md)