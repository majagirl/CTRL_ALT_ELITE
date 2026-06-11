#include "SPI.h"
#include "RF24.h"
#include "nRF24L01.h"

#define CE_PIN 9
#define CSN_PIN 10

RF24 radio(CE_PIN,CSN_PIN);

//Enum for statusen
enum Status {
  AV,
  PAA
};

enum Lamper {
  HELP,
  BREW,
  DISHES,
  ANTALL //Denne enumen er for å kunne aksessere arrayet i backroom structen på en mer lesbar måte samt at vi kan endre antall knapper lett ved å bare legge til en knapp i enumet!
}

struct Bakrom {
  int lamper[ANTALL];

  RF24% radio;
  byte adresse[6];
}

Bakrom bakrommet = {
  {2,3,4} //lamper

  radio;
}

Payload payload;

void settUpBakrommet() {
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);

  radio.setPayloadSize(32);
  radio.openReadingPipe(0,payload.adresse);
  radio.startListening();
  
  Serial.println("Satt opp bakrommet"); 
}

void settOppLamper() {
  pinMode(bakrommet.lamper[HELP], OUTPUT;
  pinMode(bakrommet.lamper[BREW], OUTPUT);
  pinMode(bakrommet.lamper[DISHES], OUTPUT);

  Serial.println("Satt opp lamper"); 
}

void radioFungerer() {
  if (!radio.isChipConnected()) {
    Serial.println("Radio ikke koblet til...."); //Sjekker om det er noe galt med radioen
    while(1);
  } else {
    Serial.println("Alt gikk fint med radioen!");
  }
}

void lesFraDisk() {
  bakrommet.radio.read(&payload,sizeof(payload));
}

void skruPaaLamper(){
  for (int i=0; i < ANTALL; i++) {
    if (payload.statuser[i] == PAA) {
      switch (payload.statuser[i]){ //Lyser kan enten være av eller på derfor switch
        case AV: //hvis lyset er av
        payload.statuser[i]= PAA;
        Serial.println("Lyset "+ i+ "ble endret til: PAA" )
        break;

        case PAA: //hvis lyset er på
        payload.statuser[i]= AV;
        Serial.println("Lyset "+ i+ "ble endret til: AV" )
        break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  lesFraDisk(); //Må lese en gang først for å kunne sette opp alt riktig
  settOppBakrommet();
  settOppLamper();
  radioFungerer();

}

void loop() {
  
  lesFraDisk();
  skruPaaLamper();
  
}





