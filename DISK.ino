//Dette er senderen, den står ved kassa 

#include "SPI.h"
#include "RF24.h"
#include "nRF24L01.h"

#define CE_PIN 9
#define CSN_PIN 10
 
#define INTERVAL_MS_TRANSMISSION 250
RF24 radio(CE_PIN, CSN_PIN);

//Enum for statusen
enum Status {
  AV,
  PAA
};

enum Knapper {
  HELP,
  BREW,
  DISHES,
  ANTALL //Denne enumen er for å kunne aksessere arrayet i backroom structen på en mer lesbar måte samt at vi kan endre antall knapper lett ved å bare legge til en knapp i enumet!
};

//Konstruerer en struct for enklere lesing
struct Disk {
  int knapper[ANTALL];
  
  RF24& radio;
  byte adresse[6];
}

//Lager en struct som skal sendes til motaker (gjennom payload), structen inneholder statusen til alle LED- lampene
struct Payload {
  Status statuser[ANTALL]; //For å kunne iterere gjennom senere
  byte adresse[6];
}

Disk disken = {
  {2,3,4}; //knapper
  
  radio; //radioen
  "123456"; //adresse
};

Payload payload = { //Inisialiserer en payload instans som vi sender senere
  {AV,AV,AV} //statuser
  "123456"; //adresse
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

  radio.openWritingPipe(disken.addresse);
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

//hjelpemetode for debounce
bool trykketPaa(int btnPin) {
	bool trykket = digitalRead(btnPin) == LOW;
    if (trykket) {
      delay(200);
       
      while (digitalRead(btnPin) == LOW) {
      	delay(10);
      }
  }
  return trykket;
}

void byttVerdi(){
  for (i=0; i < ANTALL; i++) {
    if (trykketPaa(disken.knapper[i])) { //hvis knappen er trykket på
      
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
}

void skrivTilBakrommet(){
  disken.radio.write(&payload,sizeof(payload));
}

void setup() {
  Serial.begin(115200);

  settOppKnapp();
  settOppSender();
  radioFungerer();

}

void loop() {
  
  byttVerdi();
  skrivTilBakrommet();

  delay(INTERVAL_MS_TRANSMISSION);

}
