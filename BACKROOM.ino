//Dette er vanligvis mottakeren, den sitter på bakrommet

//Importerer biblioteket til RF24 komponenten
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
  ANTALL 
};

struct Bakrom {
  int lamper[ANTALL];
  int motattKnapp;

  RF24& radio;
  byte adresse[6];
};

struct Payload {
  Status statuser[ANTALL];
};

Bakrom bakrommet = {
  {5,6,7}, //lamper
  4, //motatt knapp

  radio,
  "123456" //adresse
};

Payload payload = // lagrer verdiene med av først
{{AV,AV,AV}};

//Enumene må eksistere likt i både mottaker og sender for at logikken skal fungere

void settOppBakrommet() {
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);

  radio.openReadingPipe(0, bakrommet.adresse);  
  radio.startListening(); 
  
  Serial.println("Satt opp bakrommet"); 
}

void settOppLamper() {
  pinMode(bakrommet.lamper[HELP], OUTPUT);
  pinMode(bakrommet.lamper[BREW], OUTPUT);
  pinMode(bakrommet.lamper[DISHES], OUTPUT);
  pinMode(bakrommet.motattKnapp, INPUT_PULLUP);

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
  if (bakrommet.radio.available()) {
    bakrommet.radio.read(&payload, sizeof(payload));
    
    //Printe output som set slik ut f.eks: Mottok: 0 0 0
    Serial.print("Mottok: ");
    for (int i = 0; i < ANTALL; i++) {
      Serial.print(payload.statuser[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void slaaAvAlleLamper() { //hjelpemetode
  Payload avPayload = {{AV, AV, AV}}; //setter alle lysene til å bli av slik at vi kan bruke den samme metoden

  //bytter statusen til å bli sender
  radio.stopListening();
  radio.openWritingPipe(bakrommet.adresse);
  radio.write(&avPayload, sizeof(avPayload));

  //bytter tilbake til motaker
  radio.openReadingPipe(0, bakrommet.adresse);
  radio.startListening();

  Serial.println("Motatt knapp trykket!");
}

void sendTilDisken() {
  if (digitalRead(bakrommet.motattKnapp) == LOW) { //low pga. pullup
    
    //debounce
    while (digitalRead(bakrommet.motattKnapp) == LOW) {
      delay(5);
    }
    delay(50); 

    slaaAvAlleLamper();
  }
}

void skruPaaLamper(){
  for (int i=0; i < ANTALL; i++) {
      
      switch (payload.statuser[i]){ //Lyset kan enten være av eller på derfor switch
        case AV: //hvis lyset er av
        digitalWrite(bakrommet.lamper[i],LOW);
        break;

        case PAA: //hvis lyset er på
        digitalWrite(bakrommet.lamper[i],HIGH);
        break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  settOppBakrommet();
  settOppLamper();
  radioFungerer();

}

void loop() {
  
  lesFraDisk();
  skruPaaLamper();

  sendTilDisken();

}





