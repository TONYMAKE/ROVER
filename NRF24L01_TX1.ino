#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
 
RF24 radioTX(9,10);  //pin CE, pin CSN
const byte address[6] = "MERC8"; //costante per impostare indirizzo

int potpin[]={A0,A1,A2,A3,A6,A7}; // A0 potpin[0]= JOY SX DX/SX
                                  // A1 potpin[1]= JOY SX UP/DN
                                  // A2 potpin[2]= JOY DX DX/SX
                                  // A3 potpin[3]= JOY DX UP/DN
                                  // A6 potpin[6]= POT AUX SX
                                  // A7 potpin[7]= POT AUX DX

int pulspin[]={2,3,4,5,6,7,8};    // D2 pulspin[0]= PULS P1 JOY SX
                                  // D3 pulspin[1]= PULS P2 JOY DX
                                  // D4 pulspin[2]= PULS P3 ROSSO SX
                                  // D5 pulspin[3]= PULS P4 VERDE e S1
                                  // D6 pulspin[4]= PULS P5 NERO
                                  // D7 pulspin[5]= PULS P6 ROSSO DX
                                  // D8 pulspin[6]= PULS P7 BLU e R1

int ledrosso=18; //uso pin analogici come output: A0=14, A1=15 ... A5=19
int ledverde=19; //uso pin analogici come output: A0=14, A1=15 ... A5=19
struct miafrase{  //struttura dati da trasmettere
  int pot[6];     //6 potenziometri
  bool puls[7];   //7 pulsanti
};

struct miafrase frase;
struct miafrase fraseprec;

int cambiato=0;

int iste=1;
int zero_pot[4];


void setup() {
  
  for (int i=0;i<7;i++){    //imposto i pin dei pulsanti come INPUT
    pinMode(pulspin[i],INPUT_PULLUP);
  }
  pinMode(ledrosso,OUTPUT);
  pinMode(ledverde,OUTPUT);
  
  Serial.begin(9600);
  radioTX.begin();             //inizializzo radio
  radioTX.setChannel(124);     //imposto canale radio
  radioTX.openWritingPipe(address);  //imposto indirizzo
  radioTX.setPALevel(RF24_PA_MAX);   //imposto potenza  RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radioTX.setDataRate(RF24_250KBPS); //RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radioTX.setPayloadSize(19);
  //radioTX.setAutoAck(false);
  radioTX.stopListening();           //imposto radio in trasmissione

  zero_pot[0]=analogRead(potpin[0]);  //inizializzo variabili per azzeramento
  zero_pot[1]=analogRead(potpin[1]);  //inizializzo variabili per azzeramento
  zero_pot[2]=analogRead(potpin[2]);  //inizializzo variabili per azzeramento
  zero_pot[3]=analogRead(potpin[3]);  //inizializzo variabili per azzeramento
  zero_pot[4]=analogRead(potpin[4]);  //inizializzo variabili per azzeramento
  zero_pot[5]=analogRead(potpin[5]);  //inizializzo variabili per azzeramento

}
 
void loop() {
  fraseprec=frase;
  digitalWrite(ledrosso, 0);
  cambiato=0;
    for (int i=0; i<4; i++){       //lettura potenziometri joystick
      frase.pot[i]=analogRead(potpin[i]);
      if(frase.pot[i]>(zero_pot[i]+iste)) frase.pot[i]=map(frase.pot[i], zero_pot[i]+iste,1023, 0,4095);
        else if(frase.pot[i]<(zero_pot[i]-iste)) frase.pot[i]=map(frase.pot[i],0,zero_pot[i]-iste, -4095,0);
          else frase.pot[i]=0;
      if (fraseprec.pot[i]!=frase.pot[i]) cambiato++;
      }

    frase.pot[4]=analogRead(potpin[4]);  //lettura potenziometro aux sx
    frase.pot[4]=map(frase.pot[4], 0,1023, 0,4095);
    if (fraseprec.pot[4]!=frase.pot[4]) cambiato++;
    frase.pot[5]=analogRead(potpin[5]);    //lettura potenziometro aux dx
    frase.pot[5]=map(frase.pot[5], 0,1023, 0,4095);
    if (fraseprec.pot[5]!=frase.pot[5]) cambiato++;



    for(int i=0; i<7; i++){        //lettura pulsanti
      frase.puls[i]= !(digitalRead(pulspin[i]));
      if (fraseprec.puls[i]!=frase.puls[i]) cambiato++;
      }
  if (cambiato>0){

   bool tx_OK =radioTX.write(&frase, sizeof(frase)); //creo flag per trasmissione avvenura correttamente
 
  
  
  for (int i=0; i<6; i++){
      Serial.print(frase.pot[i]);
      Serial.print(" ");
    }
    for(int i=0; i<7; i++){
      Serial.print(frase.puls[i]);
      Serial.print(" ");
    }
    //Serial.print(frase.errcontr);*/
    
    if (tx_OK) {     // se la trasmissione è avvenuta correttamente scrivo i dati nel serial monitor
    //Serial.println(frase);
    Serial.println(" OK!!!");
    digitalWrite(ledrosso, 1);
    


  } else {       // se la trasmissione non è avvenuta correttamente scrivo messaggio di errore
    
    
    Serial.println(" Errore:  nessuna ricezione da rx");
   // digitalWrite(ledrosso, 0);
  
  }
  delay(20);
  }
  //delay(1000);
}
