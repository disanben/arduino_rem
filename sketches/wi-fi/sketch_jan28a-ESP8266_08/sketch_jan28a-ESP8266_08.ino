#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SoftwareSerial.h>
#include "DHT.h"
#include <ESP8266WiFi.h>

#include <Wire.h>;
#include <LiquidCrystal_I2C.h>;
//#include <LCD03.h>;

void getStatoStove(char *response);
void getStatoFan(char *response);

int iNumWiFi=4;
//dimensiono per 10 reti WiFi
char *ssidWiFi[10][20];
char *pswdWiFi[10][20];
char *ssid;
char *password;

const int httpPort = 80;

/*For LocalHost*/
//String szHost="192.168.43.141";               //ssid = BenMiA2
//String szHost="http://192.168.23.120";        //ssid = AP_IOCENTRO

/* For External Host*/
String szHost="www.habita3th.eu";

//Stringhe per CURL delle richieste
//Ritorna lo stato della STOVE. 1 se ON, 0 se OFF
String urlReq01="/iot/php/getStato.php?req_Type=01&id_req=01";
//Setta lo stato della STOVE [00 Spenta, 01 Accesa]
String urlReq10="/iot/php/setStato.php?req_Type=02&id_req=00";
String urlReq11="/iot/php/setStato.php?req_Type=02&id_req=01";

//Ritorna lo stato del FAN. 1 se ON, 0 se OFF
String urlReq02="/iot/php/getStato.php?req_Type=01&id_req=02";
//Setta lo stato della FAN [10 Spenta, 11 Accesa]
String urlReq12="/iot/php/setStato.php?req_Type=02&id_req=10";
String urlReq13="/iot/php/setStato.php?req_Type=02&id_req=11";

//Setta Temperatura (TT) e Humidita (HH) corrente
//String urlReq15="/iot/setParApp.php?req_Type=04&app_temp=TT&app_humy=HH";
String urlReq15="/iot/php/setParApp.php?req_Type=04&app_temp=";

//Tipo di risposte
//char cStoveResp[]="01|1|STOVE|1|1|22|25|-|-|-|2019-02-08 23:57:17";
//char cFanResp[]="01|2|FAN|0|-|-|-|1|82|60|2019-02-08 23:57:17";

char cDelim[]="|";
char **arStatoStove = (char**)malloc(15*sizeof(char**));
char **arStatoFan = (char**)malloc(15*sizeof(char*));

float fTempRif=0; //Temperatura di riferimento impostata
float fTempNow=0; //Temperatura attuale rilevata dal sensore

float fHumyRif=0; //Humidita di riferimento impostata
float fHumyNow=0; //Humidita di attuale rilevata dal sensore

int pinTempHumyIn   = 4; //GPIO4 (D2) Pin di input sensore Temp. ed Humy

int ledPinStove     = 14; //GPIO14 (D5) red - Led di stato Acceso/Spento
int ledPinAutoStove = 12; //GPIO12 (D6) green - Let Auto ON/OFF
int ledPinFan       = 13; //GPIO13 (D7) yellow
int ledPinAutoFan   = 15; //GPIO15 (D8) green - Let Auto ON/OFF

//int valueStove=0;
//int valueFan=0;

DHT dht; //Sensore DHT22

//WiFiServer server(80);
WiFiClient client;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

   Serial.println("+++ Inizio SETUP +++");
   int i=0;
   int j=0;
   int iWiFi=0;
  
   *ssidWiFi[10][20];
   *pswdWiFi[10][20];

   *ssidWiFi[0] = "BenMiA2";
   *pswdWiFi[0] = "benedetto";
   *ssidWiFi[1] = "AP_IOCENTRO";
   *pswdWiFi[1] = "ioCentro1";
   *ssidWiFi[2] = "TP-LINK_9D4E33";
   *pswdWiFi[2] = "benedetto";
   *ssidWiFi[3] = "ben-wifi";
   *pswdWiFi[3] = "benedetto";
  
   // put your setup code here, to run once:
   Serial.begin(115200);
   //Serial.begin(9600); //per sensore DHT22

   Serial.println("Test LCD 0");
   
   lcd.begin();
   Serial.println("Test LCD 1");
   
   lcd.backlight();
   Serial.println("Test LCD 2");

   //lcd.setCursor(0,0);
   //Serial.println("Test LCD 3");
   
   lcd.print("Temp  ");
   Serial.println("Test LCD 4");
   lcd.print("Humidity");
   Serial.println("Test LCD 5");
   
   delay(100);

   //Pin per lettura sensore
   pinMode(pinTempHumyIn, INPUT);
  
   //Led di STATO della STOVE. Inizializzato spento
   pinMode(ledPinStove, OUTPUT);
   digitalWrite(ledPinStove, LOW);
  
   pinMode(ledPinFan, OUTPUT);
   digitalWrite(ledPinFan, LOW);

   pinMode(ledPinAutoStove, OUTPUT);
   digitalWrite(ledPinAutoStove, LOW);

   pinMode(ledPinAutoFan, OUTPUT);
   digitalWrite(ledPinAutoFan, LOW);
 


   //Led di STATO del FAN. Inizializzato spento
   //pinMode(ledPinFan, OUTPUT);
   //digitalWrite(ledPinFan, LOW);

   //Inizializzo il sensore al pin di lettura
   dht.setup(pinTempHumyIn);
  
   Serial.println("+++Inizio cicloo for per 4 reti+++");
   for(i=0; i<4; i++){
      ssid=*ssidWiFi[i];
      password=*pswdWiFi[i];
      Serial.print("Try to connect in: ");
      Serial.println(ssid);
      WiFi.begin(ssid, password);
      j=0;
      iWiFi=1;
      Serial.println("+++ Inizio while +++");
      while(WiFi.status() != WL_CONNECTED){
         delay(500);
         Serial.print(".");
         j=j+1;
         if(j>20){
            iWiFi=0;
            Serial.println("--- Esco da while con break. Rete non connessa ---");
            break;
         }
      }
      Serial.print("--- Esco da while con iWiFi: ");
      Serial.print(iWiFi);
      Serial.println(" ---");

      //Set rete e password               
      if(iWiFi == 1){
         Serial.println("--- Esco da ciclo for per 4 reti perchè ho trovato la rete ---");
         break;
      }
   }
   Serial.println("---Fine ciclo for per 4 reti---");
   if(iWiFi == 0){
      Serial.print("Non trovate reti WiFi a cui connettersi. EXIT!!!");
      exit;
   }

   //Assegno la rete trovata
   Serial.print("WiFi Connected with ssid: ");
   Serial.print(ssid);
   Serial.print(" e psw: ");
   Serial.println(password);

   Serial.println("+++ Fine SETUP +++");
}




void loop() {
   delay(5000);

   
   //Legge i dati del sensore
   float fHumyNow = dht.getHumidity();
   float fTempNow = dht.getTemperature();

   lcd.setCursor(0,1);
   
   lcd.print(fTempNow);
   lcd.print("    ");
   lcd.print(fHumyNow);
   
   String szLettura=dht.getStatusString();
   Serial.println("Lettura del sensore: " + szLettura);

   //------ Inizio invio Clima a B.E. ----------
   //Invia al B.E. i dati di Temperatura ed Umidita con urlReq15
   Serial.println("Connecting to: " + szHost + ":" + httpPort);
   if (!client.connect(szHost, httpPort)) {
      Serial.println("connection failed");
      return;
   }
   Serial.println("Invio dell' aggiornamento di clima della stanza");
   Serial.println("GET " + urlReq15 + fTempNow + "&app_humy=" + fHumyNow + " HTTP/1.1\r\n" +
                  "Host: " + szHost + "\r\n" + 
                  "Connection: close\r\n\r\n");
               
   client.print(String("GET ") + urlReq15 + fTempNow + "&app_humy=" + fHumyNow + " HTTP/1.1\r\n" +
                "Host: " + szHost + "\r\n" + 
                "Connection: close\r\n\r\n");

   //------ Fine invio Clima a B.E. ----------

   String responseStove="";
   String responseFan="";
   Serial.println("##############  START LOOP #####################");
   Serial.println("Connecting to: " + szHost + ":" + httpPort);
   if (!client.connect(szHost, httpPort)) {
      Serial.println("connection failed");
      return;
   }

   ///////////////////////////////////////////////////////////
   // This will send the request to the server for the STOVE
   ///////////////////////////////////////////////////////////
   Serial.println("Invio della richiesta per stato Stufa");
  
   Serial.println("GET " + urlReq01 + " HTTP/1.1\r\n" +
                  "Host: " + szHost + "\r\n" + 
                  "Connection: close\r\n\r\n");
               
   client.print(String("GET ") + urlReq01 + " HTTP/1.1\r\n" +
               "Host: " + szHost + "\r\n" + 
               "Connection: close\r\n\r\n");
                 
   // Read all the lines of the reply from server and print them to Serial

   delay(1000);

   while(client.available()){
      char c = client.read(); //gets byte from ethernet buffer
      responseStove += c; //places captured byte in readString
   }
   
   client.flush();
   Serial.println("Respond from STOVE request:");
   Serial.println("Analizzo la risposta: " + responseStove);
   char* myResponseStove = const_cast<char*>(responseStove.c_str());

   if(!strstr(myResponseStove, "STOVE")>0){
      Serial.print("Not find STOVE records. Exit from while loop.");
   }else{ 
      getStatoStove(myResponseStove);
      //Verifica lo stato Acceso/Spento
      Serial.print("Analizzo la risposta per lo stato di STOVE: ");
      Serial.println(*arStatoStove[3]);
      if(*arStatoStove[3] == '1'){
         Serial.println("STOVE: Trovato STATO=1. Stufa Accesa");
         digitalWrite(ledPinStove, HIGH);
      } else {
         Serial.println("STOVE: Trovato STATO=O. Stufa spenta");
         digitalWrite(ledPinStove, LOW);
      }

      //Verifica se è in AUTO Mode
      //In modalità AUTO la stufa è gestita da questo firmware e non dall'app in manuale
      Serial.print("Analizzo la risposta per modalita AUTO di STOVE: ");
      Serial.println(*arStatoStove[4]);
      if(*arStatoStove[4] == '1'){
         Serial.println("STOVE: Trovato AUTO=1. Stufa in modalita AUTO");
         digitalWrite(ledPinAutoStove, HIGH);
         //Lettura della temperatura attuale e di riferimento per gestire lo stato della stufa
         fTempRif=atof(arStatoStove[6]);
         
         //Confronta la temperatura della Stufa con quella di riferimento
         if(fTempNow < fTempRif){
            Serial.print("Temperatura minore del minimo di riferimento: ");
            Serial.print(fTempRif);
            Serial.print(" - Attuale: ");
            Serial.println(fTempNow);
            //Verifico se la stufa è spenta
            if(*arStatoStove[3] == '0'){
               digitalWrite(ledPinStove, HIGH);
               //Invia nuovo stato ON al B.E. con urlReq11
               Serial.println("Connecting to: " + szHost + ":" + httpPort);
               if (!client.connect(szHost, httpPort)) {
                  Serial.println("connection failed");
                  return;
               }
               Serial.println("Invio dell' aggiornamento di stato ON della stufa");
               Serial.println("GET " + urlReq11 + " HTTP/1.1\r\n" +
                              "Host: " + szHost + "\r\n" + 
                              "Connection: close\r\n\r\n");
               
               client.print(String("GET ") + urlReq11 + " HTTP/1.1\r\n" +
                             "Host: " + szHost + "\r\n" + 
                             "Connection: close\r\n\r\n");
         }
      } else{
            if(*arStatoStove[3] == '1'){
            digitalWrite(ledPinStove, LOW);
            //Invia nuovo stato OFF al B.E. con urlReq10
            Serial.println("Connecting to: " + szHost + ":" + httpPort);
            if (!client.connect(szHost, httpPort)) {
               Serial.println("connection failed");
               return;
            }
            Serial.println("Invio dell' aggiornamento di stato ON della stufa");
            Serial.println("GET " + urlReq10 + " HTTP/1.1\r\n" +
                           "Host: " + szHost + "\r\n" + 
                           "Connection: close\r\n\r\n");
               
            client.print(String("GET ") + urlReq10 + " HTTP/1.1\r\n" +
                        "Host: " + szHost + "\r\n" + 
                        "Connection: close\r\n\r\n");
         }
         Serial.print("Temperatura maggiore del minimo di riferimento: ");
         Serial.print(fTempRif);
         Serial.print(" - Attuale: ");
         Serial.println(fTempNow);
      }
   } else{
      Serial.println("STOVE: Trovato AUTO=O. Stufa in manuale da APP");
     digitalWrite(ledPinAutoStove, LOW);
   }


   ///////////////////////////////////////////////////////////
   // This will send the request to the server for the FAN
   ///////////////////////////////////////////////////////////
   Serial.println("Connecting to: " + szHost + ":" + httpPort);
   if (!client.connect(szHost, httpPort)) {
      Serial.println("connection failed");
      return;
   }

   Serial.println("Invio della richiesta per stato Ventilatore");
   Serial.println("GET " + urlReq02 + " HTTP/1.1\r\n" +
                  "Host: " + szHost + "\r\n" + 
                  "Connection: close\r\n\r\n");
               
   client.print(String("GET ") + urlReq02 + " HTTP/1.1\r\n" +
                "Host: " + szHost + "\r\n" + 
                "Connection: close\r\n\r\n");
                 
   // Read all the lines of the reply from server and print them to Serial

   delay(1000);

   while(client.available()){
      char f = client.read(); //gets byte from ethernet buffer
      responseFan += f; //places captured byte in readString
   }
   client.flush();
   Serial.println("Respond from FAN request:");
   Serial.println("Analizzo la risposta: " + responseFan);

   char* myResponseFan = const_cast<char*>(responseFan.c_str());

   if(!strstr(myResponseFan, "FAN")>0){
      Serial.print("Not find FAN records. Exit from while loop.");
   } else{
      getStatoFan(myResponseFan);
      Serial.print("Analizzo la risposta per lo stato di FAN: ");
      Serial.println(*arStatoFan[3]);
      if(*arStatoFan[3] == '1'){
         Serial.println("FAN: Trovato STATO=1. Ventilatore Acceso");
         digitalWrite(ledPinFan, HIGH);
      } else{
         Serial.println("FAN: Trovato STATO=O. Ventilatore spento");
         digitalWrite(ledPinFan, LOW);
      }

      //Verifica se è in AUTO Mode
      //   In modalità AUTO il ventilatore è gestito da questo firmware e non dall'app in manuale
      Serial.print("Analizzo la risposta per modalita AUTO di FAN: ");
      Serial.println(*arStatoFan[7]);
      if(*arStatoFan[7] == '1'){
         Serial.println("FAN: Trovato AUTO=1. Ventola in modalita AUTO");
         digitalWrite(ledPinAutoFan, HIGH);
         //Lettura della umidità attuale e di riferimento per gestire lo stato della ventola
         fHumyRif=atof(arStatoFan[9]);
         //Confronta la temperatura della Stufa con quella di riferimento
         if(fHumyNow > fHumyRif){
            Serial.print("Umidità maggiore del massimo di riferimento: ");
            Serial.print(fHumyRif);
            Serial.print(" - Attuale: ");
            Serial.println(fHumyNow);
            //Verifico se la ventola e' spenta
            if(*arStatoFan[3] == '0'){
               digitalWrite(ledPinFan, HIGH);
               //Invia nuovo stato ON al B.E. con urlReq13
               Serial.println("Connecting to: " + szHost + ":" + httpPort);
               if (!client.connect(szHost, httpPort)) {
                  Serial.println("connection failed");
                  return;
               }
               Serial.println("Invio dell' aggiornamento di stato ON della ventola");
               Serial.println("GET " + urlReq13 + " HTTP/1.1\r\n" +
                              "Host: " + szHost + "\r\n" + 
                              "Connection: close\r\n\r\n");
               
               client.print(String("GET ") + urlReq13 + " HTTP/1.1\r\n" +
                            "Host: " + szHost + "\r\n" + 
                            "Connection: close\r\n\r\n");
            }

         } else{
            //Verifico se la ventola e' accesa
            if(*arStatoFan[3] == '1'){
               digitalWrite(ledPinFan, LOW);
               //Invia nuovo stato OFF al B.E. con urlReq12
               Serial.println("Connecting to: " + szHost + ":" + httpPort);
               if (!client.connect(szHost, httpPort)) {
                  Serial.println("connection failed");
                  return;
               }
               Serial.println("Invio dell' aggiornamento di stato ON della ventola");
               Serial.println("GET " + urlReq12 + " HTTP/1.1\r\n" +
                              "Host: " + szHost + "\r\n" + 
                              "Connection: close\r\n\r\n");
               
               client.print(String("GET ") + urlReq12 + " HTTP/1.1\r\n" +
                            "Host: " + szHost + "\r\n" + 
                            "Connection: close\r\n\r\n");
            }
            Serial.print("Umidità minore del massimo di riferimento: ");
            Serial.print(fHumyRif);
            Serial.print(" - Attuale: ");
            Serial.println(fHumyNow);
         }
      }else {
         Serial.println("FAN: Trovato AUTO=O. Ventola in manuale da APP");
         digitalWrite(ledPinAutoFan, LOW);
      }

      yield();
      Serial.println("Client Disconnected");
      Serial.println("");
      Serial.println("----------------  END LOOP ------------------");
   }
}
}

//Funzione che recupera l'array con i valori dell'appliance
void getStatoStove(char* myResponse){
    char *ptr;
    int i=0;

    Serial.print("getStato: risposta arrivata: ");
    Serial.println(myResponse);
    ptr=strtok(myResponse, cDelim);
    while (ptr != NULL){
      arStatoStove[i++] = ptr;
      ptr = strtok (NULL, "|");
    }

    Serial.println("\ngetStatoStove: ******* Parametri Trovati [STOVE] ********\n");
  
    for (i = 0; i < 15; ++i)  {
       Serial.println(arStatoStove[i]);  
    }

    yield();
}

//Funzione che recupera l'array con i valori dell'appliance
void getStatoFan(char* myResponse){
    char *ptr;
    int i=0;

    Serial.print("getStato: risposta arrivata: ");
    Serial.println(myResponse);
    ptr=strtok(myResponse, cDelim);
    while (ptr != NULL){
      arStatoFan[i++] = ptr;
      ptr = strtok (NULL, "|");
    }

    Serial.println("\ngetStatoFan: ******* Parametri Trovati [FAN] ********\n");
  
    for (i = 0; i < 15; ++i)  {
       Serial.println(arStatoFan[i]);  
    }
    yield();
}

  /*
  TO DO:
     - Ripulire il codice
     - Fare uscire dal setup se non trova la rete
     - Creare entry per il sensore di umidità e temperatura 
     - Implementare gestione dell'AUTO
  */
  
