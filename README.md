# CTRL_ALT_ELITE
Prosjektet vårt tar utgangspunkt i kafeen Supreme Roastworks på Grünerløkka. Gjennom semesteret har vi 
utviklet en teknologisk løsning som kan støtte intern kommunikasjon mellom ansatte i kaffebaren og ansatte 
som har pause. Målet var å bedre kommunikasjonen uten å forstyrre arbeidsflyten.

Den tekniske løsningen består av to filen:
BACKROOM.ino
DISK.ino

Den tekniske løsningen er spredt over to Arduino UNOer og er ment til å effektivisere kommunikasjonen mellom 
den som sitter på bakrommet og den som har pause og den som står i disken. Arduinoene kommuniserer via en RF24 
radio komponent. Radioen takler bare at det er en mottaker og sender av gangen, dette måtte vi ta betraktning i 
løsningen. 

Arduino 1, som sitter på bakrommet, er utstyrt med tre LED-lys og en «mottatt» knapp som skrur alle lysene av. 
På Arduino 2, som sitter i disken, er det tre knapper. Knappene i disken korresponderer med lys på bakrommet. 
Knappene er kategorisert basert på problemer de ansette ofte trenger hjelp med, i dette tilfelle er kategoriene; 
HJELP, BREW og DISHES. Når de bak disken trykker på en knapp lyser eller skur av korresponderende den LED-lampen på 
bakrommet. Da slipper den som står i disken å avbryte arbeidet for å varsle den som har pause om at de trenger hjelp med noe.
