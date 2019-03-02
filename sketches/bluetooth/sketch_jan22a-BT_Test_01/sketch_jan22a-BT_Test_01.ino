//Code begin here
#include <SoftwareSerial.h>

SoftwareSerial BT(10,11); //RX, TX rispettivamente
String state; //Memorizza messaggi da bluetooth

void setup(){
  pinMode(13, OUTPUT); //Led connesso al pin 13
  pinMode(10, INPUT);
  pinMode(11, OUTPUT);
  
  BT.begin (9600); //Bluetooth serial prenderà i pin 10 e 11
  Serial.begin(9600); //Serial communication con il monitor seriale
}


void loop(){
  //Verifica se c'e' qualcosa da leggere nel BlueTooth
  //Serial.print("Sono nel loop ");
  while (BT.available()){
    Serial.print("Just arrived: ");
    delay(10); //Delay added to make thing stable
    char c = BT.read(); //Lettura seriale
    Serial.println(c);
    state += c; //Costruisce la stringa ON o OFF
  }

  if(state.length()>0){
    Serial.println(state);
    
    if(state == "ON"){
      digitalWrite(13, HIGH);
      BT.print("Luce Accesa");
      Serial.println("Luce Accesa");
      }
    else if(state == "OFF"){
      digitalWrite(13, LOW);
      BT.print("Luce Spenta");
      Serial.println("Luce Spenta");
    }

    state="";
  }
}
//Fine del codice
