#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
 
RF24 radioRX(9,10); //pin CE, pin CSN
const byte address[6] = "MERC8"; //costante per impostare indirizzo

struct miafrase{//struttura dati da ricevere
  int pot[6];     //6 potenziometri
  bool puls[7];   //7 pulsanti
  //int errcontr;   // parola di controllo (somma di tutti i potenziometri e pulsanti)
};

long tempoprec;
long tempoattuale;
long tempo;

miafrase frase;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
int channel = 0;

int chspeedsx=0;//ANT
int chversosx=1;//ANT
int chspeeddx=2;//ANT
int chversodx=3;//ANT
//POST= +4

int joyy=0;
int joyx=0;

int speedmotsx=0;
int speedmotdx=0;
bool versomotsx=0;
bool versomotdx=0;


void setup() {
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency

  Serial.begin(9600);
  radioRX.begin();                    //inizializzo radio
  radioRX.setChannel(124);            //imposto canale radio
  radioRX.openReadingPipe(1, address);//imposto indirizzo
  radioRX.setPALevel(RF24_PA_MAX);    //imposto potenza  RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radioRX.setDataRate(RF24_250KBPS);
  radioRX.setPayloadSize(19);
  //radioRX.setAutoAck(false);
  radioRX.startListening();           //imposto radio in ricezione
  tempoattuale=millis();  //per debug velocità ricezione
  tempo=0;                //per debug velocità ricezione
  tempoprec=millis();     //per debug velocità ricezione
  pinMode(3, OUTPUT);     //per debug velocità ricezione
}

void loop() {
  //char msg[32] = {0};
  //int msg=0;
  
  if (radioRX.available()) {            //se ci sono dati in ricezione
  radioRX.read(&frase, sizeof(frase));//leggi i dati
    //analogWrite(3, abs(frase.pot[0]));
  //pwm.setPWM(channel, 0, map(abs(frase.pot[5]),0,255,0,4095) );
  //pwm.setPWM(15, 0, map(abs(frase.pot[1]),0,255,0,4095));
  pwm.setPWM(15, 0, abs(frase.pot[1]));
  
  
  //if(frase.pot[1]<0) pwm.setPWM(1, 0, 4095);
  //if(frase.pot[1]>0) pwm.setPWM(1, 0, 0);
  //pwm.setPWM(0, 0, abs(frase.pot[1]));
  joyy=frase.pot[1];
  joyx=frase.pot[0];
  speedmotsx=joyy-joyx;
  speedmotdx=joyy+joyx;
  
  if (speedmotsx<0) versomotsx=1;
  if (speedmotsx>0) versomotsx=0;
  if (speedmotdx<0) versomotdx=0;
  if (speedmotdx>0) versomotdx=1;
  speedmotsx=constrain(speedmotsx, -4095, 4095);
  speedmotdx=constrain(speedmotdx, -4095, 4095);
  
   pwm.setPWM(chspeedsx, 0, abs(speedmotsx));
   pwm.setPWM(chversosx,0,versomotsx*4095);
   pwm.setPWM(chspeeddx, 0, abs(speedmotdx));
   pwm.setPWM(chversodx,0,versomotdx*4095);
   pwm.setPWM(chspeedsx+4, 0, abs(speedmotsx));
   pwm.setPWM(chversosx+4,0,versomotsx*4095);
   pwm.setPWM(chspeeddx+4, 0, abs(speedmotdx));
   pwm.setPWM(chversodx+4,0,versomotdx*4095);
 
  
  
  
  
 //delay(10);
  
  
  
  
  tempoattuale=millis();             //per debug velocità ricezionee
    tempo=tempoattuale-tempoprec;      //per debug velocità ricezione
    tempoprec=tempoattuale;            //per debug velocità ricezione
    Serial.print(tempo);               //per debug velocità ricezione  
    Serial.print("  ");               //per debug velocità ricezione
    Serial.print(1000/tempo);          //per debug velocità ricezione 
    Serial.print("  ");               //per debug velocità ricezione

  Serial.print(speedmotdx);
  Serial.print("  ");
  Serial.print(speedmotsx);
  Serial.print("  ");

  
    //Serial.println(msg);
    for (int i=0; i<6; i++){             //visualizza dati potenziometri
      Serial.print(frase.pot[i]);
      Serial.print(" ");
    }
    for(int i=0; i<7; i++){              //visualizza dati pulsanti
      Serial.print(frase.puls[i]);
      Serial.print(" ");
    }
    //Serial.println(frase.errcontr);      //visualizza parola di controllo
    Serial.println("");      //visualizza parola di controllo
  }
  
}
