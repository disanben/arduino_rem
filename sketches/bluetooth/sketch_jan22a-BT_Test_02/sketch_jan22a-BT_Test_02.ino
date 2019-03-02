//Code begin here
#include <SoftwareSerial.h>
#include "DHT.h"
 
SoftwareSerial BT(10,11); //RX, TX rispettivamente
String request; //Memorizza messaggi da bluetooth
String response; //Invia messaggi a Bluetooth
char szTemp[6];
char szHumy[6];
String szLettura;
//String szTemp;
//String szHumy;

DHT dht; //Sensore DHT22
float fMinTemp = 25;
float fMaxHumy = 60;

void setup(){
  pinMode(8, INPUT);  //DHT22: Lettura dati
  pinMode(2, OUTPUT); //Led Rosso connesso al pin 2: Acceso se Stufa ON
  pinMode(4, OUTPUT); //Led Verde connesso al pin 4: Acceso se Ventola ON
  pinMode(10, INPUT);
  pinMode(11, OUTPUT);
  
  BT.begin (9600); //Bluetooth serial prenderà i pin 10 e 11
  Serial.begin(9600); //Serial communication con il monitor seriale

  dht.setup(8); // data pin 8
}


void loop(){
  //ritardo
  delay(5000);
  //Legge i dati del sensore
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  szLettura=dht.getStatusString();
  Serial.println("Lettura: " + szLettura);

  //Imposta gli stati della stufa e della ventola

  //Verifica Stufa
  if(temperature < fMinTemp){
    response = "[ST1] ";
    response = response + "ON";
    digitalWrite(2, HIGH);
    Serial.print("Temperatura minore del minimo: ");
    Serial.println(temperature);
    //Invio del messaggio all'app
    BT.println(response);
  }else {
    response = "[ST1] ";
    response = response + "OFF";
    digitalWrite(2, LOW);
    Serial.print("Temperatura maggiore del minimo: ");
    Serial.println(temperature);
    //Invio del messaggio all'app
    BT.println(response);
  }

  //Verifica per ventola
  if(humidity > fMaxHumy){
    response = "[ST2] ";
    response = response + "ON";
    digitalWrite(4, HIGH);
    Serial.print("Umidita maggiore del massimo: ");
    Serial.println(humidity);
    //Invio del messaggio all'app
    BT.println(response);
  }else{
    response = "[ST2] ";
    response = response + "OFF";
    digitalWrite(4, LOW);
    Serial.print("Umidita minore del massimo: ");
    Serial.println(humidity);
    //Invio del messaggio all'app
    BT.println(response);
  }
  
  
  //Verifica se c'e' qualcosa da leggere nel BlueTooth
  //Serial.print("Sono nel loop ");
  while (BT.available()){
    Serial.print("Just arrived: ");
    delay(10); //Delay added to make thing stable
    char c = BT.read(); //Lettura seriale
    Serial.println(c);
    request += c; //Costruisce la stringa ON o OFF
  }

  if(request.length()>0){
    Serial.println(request);
    //Casistica delle richieste
    //[REQ] 01: Richiesta lettura sensore
    //[REQ] 02: 
    //[REQ] 03: 
    //[REQ] 04:
    //[REQ] 05:
    //
    if(request == "[REQ] 01"){
      //Viene richiesta la lettura dello stato
      response = "[CHK] ";
      response = response + temperature + " °C - " + humidity + "%";
      //Valutare se andare a verificare tutto di nuovo
      BT.println(response);
      Serial.println(response);
      }

      if(request == "[REQ] 02"){
      //Viene richiesto lo start della stufa
      //response = "[CHK] ";
      //response = response + temperature + " °C - " + humidity + "%";
      //Valutare se andare a verificare tutto di nuovo
      //BT.println(response);
      //Serial.println(response);
      digitalWrite(2, HIGH);
      Serial.println("Stufa accesa");
      }

      if(request == "[REQ] 03"){
      //Viene richiesto lo stop della stufa
      //response = "[CHK] ";
      //response = response + temperature + " °C - " + humidity + "%";
      //Valutare se andare a verificare tutto di nuovo
      //BT.println(response);
      //Serial.println(response);
      digitalWrite(2, LOW);
      Serial.println("Stufa spenta");
      }

      if(request == "[REQ] 04"){
      //Viene richiesto lo start della ventola
      //response = "[CHK] ";
      //response = response + temperature + " °C - " + humidity + "%";
      //Valutare se andare a verificare tutto di nuovo
      //BT.println(response);
      //Serial.println(response);
      digitalWrite(4, HIGH);
      Serial.println("Ventola accesa");
      }

      if(request == "[REQ] 05"){
      //Viene richiesto lo stop della ventola
      //response = "[CHK] ";
      //response = response + temperature + " °C - " + humidity + "%";
      //Valutare se andare a verificare tutto di nuovo
      //BT.println(response);
      //Serial.println(response);
      digitalWrite(4, HIGH);
      Serial.println("Ventola spenta");
      }

    request="";
  }
}
//Fine del codice


