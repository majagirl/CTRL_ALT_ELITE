//Dette er vanligvis senderen, den står ved kassa

//Importerer biblioteket til RF24 komponenten
#include "SPI.h"
#include "RF24.h"
#include "nRF24L01.h"

#define CE_PIN 9
#define CSN_PIN 10
 
#define INTERVAL_MS_TRANSMISSION 10
RF24 radio(CE_PIN, CSN_PIN);

//Enum for statusen
enum Status {
  AV, //0
  PAA //1
};

enum Knapper {
  HELP,
  BREW,
  DISHES,
  ANTALL //Denne enumen er for å kunne aksessere arrayet på en mer lesbar måte samt at vi kan endre antall knapper lett ved å bare legge til en knapp i enumet!
};

//Konstruerer en struct for enklere lesing
struct Disk {
  int knapper[ANTALL];
  
  RF24& radio;
  byte adresse[6];
};

//Lager en struct som skal sendes til motaker (gjennom payload), structen inneholder statusen til alle LED- lampene
struct Payload {
  Status statuser[ANTALL]; //For å kunne iterere gjennom senere
};

Disk disken = {
  {2,3,4}, //knapper

  radio, //radioen
  "123456" //adresse
};

Payload payload = { //Inisialiserer en payload instans som vi sender senere
  {AV,AV,AV}, //statuser
}; 

void settOppKnapp(){
  pinMode(disken.knapper[HELP], INPUT_PULLUP);
  pinMode(disken.knapper[BREW], INPUT_PULLUP);
  pinMode(disken.knapper[DISHES], INPUT_PULLUP);

  Serial.println("Satt opp knapper"); 
}

void settOppSender() {
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  
  radio.openWritingPipe(disken.adresse);
  radio.stopListening();
    
  Serial.println("Satt opp sender");
}

void radioFungerer() {
  if (!radio.isChipConnected()) {
    Serial.println("Radio ikke koblet til...."); //Sjekker om det er noe galt med radioen
    while(1);
  
  } else {
    Serial.println("Alt gikk fint med radioen!");
  }
}

void lesFraBakrommet() {
  //bytt til å bli mottaker
  radio.openReadingPipe(0, disken.adresse);
  radio.startListening();

  unsigned long start = millis();
  while (millis() - start < 100) {  // lytt i 100ms slik at programmet kan kjøre i bakrunnen, men lytter for om "motatt" knappen er trykket
    if (radio.available()) {
      radio.read(&payload, sizeof(payload));
      Serial.println("Mottatt knapp trykket!");
    }
  }

  // bytt tilbake til sender
  radio.openWritingPipe(disken.adresse);
  radio.stopListening();
}

//hjelpemetode for debounce
bool trykketPaa(int btnPin) {
  bool trykket = digitalRead(btnPin) == LOW;
  if (trykket) {
    while (digitalRead(btnPin) == LOW) {
       skrivTilBakrommet();
      delay(5);
    }
  }
  return trykket;
}

void byttVerdi(){
  for (int i=0; i < ANTALL; i++) {
    if (trykketPaa(disken.knapper[i])) {
      
      //Skru av hvis på og visa versa
      switch (payload.statuser[i]){ //Lyset kan enten være av eller på derfor switch
        case AV:
          payload.statuser[i] = PAA;
          break;
        case PAA:
          payload.statuser[i] = AV;
          break;
      }
    }
  }
}

void skrivTilBakrommet(){
  disken.radio.write(&payload, sizeof(payload));
}

void setup() {
  Serial.begin(115200);

  settOppKnapp();
  settOppSender();
  radioFungerer();

}

void loop() {
  lesFraBakrommet();

  byttVerdi();
  skrivTilBakrommet();

  delay(INTERVAL_MS_TRANSMISSION);

}
