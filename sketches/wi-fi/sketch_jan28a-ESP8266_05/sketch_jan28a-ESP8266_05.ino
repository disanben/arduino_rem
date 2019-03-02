#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ESP8266WiFi.h>

void getStatoStove(char *response);
void getStatoFan(char *response);

const char* ssid = "AP_IOCENTRO";
const char* password = "ioCentro1";

//const char* ssid = "BenMiA2";
//const char* password = "benedetto";


//const char* ssid = "TP-LINK_9D4E33";
//const char* password = "benedetto";

//const char* ssid     = "ben-wifi";
//const char* password = "benedetto";

const int httpPort = 80;

/*For LocalHost*/
//String szHost="192.168.43.141";               //ssid = BenMiA2
//String szHost="http://192.168.23.120";        //ssid = AP_IOCENTRO

//Stringhe per CURL delle richieste
//String urlReq01="/iot/getStato.php?id_req=01";
//String urlReq02="/iot/getStato.php?id_req=02";

/* For External Host*/
String szHost="www.habita3th.eu";

//Stringhe per CURL delle richieste
//Ritorna lo stato della stufa. 1 se ON, 0 se OFF
String urlReq01="/iot/getStato.php?req_Type=01&id_req=01";
char cStoveResp[]="01|1|STOVE|1|1|22|25|-|-|-|2019-02-08 23:57:17";

//Ritorna lo stato della ventola. 1 se ON, 0 se OFF
String urlReq02="/iot/getStato.php?req_Type=01&id_req=02";
char cFanResp[]="01|2|FAN|0|-|-|-|1|82|60|2019-02-08 23:57:17";

//char response = (char*)malloc(100*sizeof(char*));
//----------------------
//const char* mstr="";
//char* str=const_cast<char*>(mstr)
//----------------------
//const char* mstr="";
//const char* response=const_cast<char*>(mstr);
char cDelim[]="|";

//char *myResponse = (char*)malloc(100*sizeof(char*));
//char *myResponse;

//char **arStatoStove = (char**)malloc(15*sizeof(char*));
//char** arStatoStove;
char **arStatoStove = (char**)malloc(15*sizeof(char**));
char **arStatoFan = (char**)malloc(15*sizeof(char*));

int ledPinStove = 12; //GPIO12 (D6) red
int ledPinFan = 14;   //GPIO14 (D5) yellow
int valueStove=0;
int valueFan=0;

//WiFiServer server(80);
WiFiClient client;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);

  pinMode(ledPinStove, OUTPUT);
  digitalWrite(ledPinStove, LOW);

  pinMode(ledPinFan, OUTPUT);
  digitalWrite(ledPinFan, LOW);

  //Connect to Wi Fi Network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    }
  Serial.println("WiFi Connected");
}

void loop() {
  delay(5000);
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
    //delay(1000);
    //String response1 = client.readStringUntil('\r');
    //String response = client.readStringUntil('\r');
    //String response = "01|1|STOVE|1|1|22|25|-|-|-|2019-02-08 23:57:17";

    char c = client.read(); //gets byte from ethernet buffer
    responseStove += c; //places captured byte in readString
  }
    client.flush();
    Serial.println("Respond from STOVE request:");
    Serial.println("Analizzo la risposta: " + responseStove);
    //Effettuo chiamata per convertire la risposta in array di stringhe
    //Recupero lo stato
    //char *myResponse;
    //response.toCharArray(myResponse, 100);
    char* myResponseStove = const_cast<char*>(responseStove.c_str());

    if(!strstr(myResponseStove, "STOVE")>0){
      Serial.print("Not find STOVE records. Exit from while loop.");
    }else{
    
       getStatoStove(myResponseStove);
    
       Serial.print("Analizzo la risposta per lo stato di STOVE: ");
       Serial.println(*arStatoStove[3]);
       if(*arStatoStove[3] == '1'){
       Serial.println("STOVE: Trovato STATO=1. Stufa Accesa");
       digitalWrite(ledPinStove, HIGH);
       valueStove = HIGH;
       } else {
       Serial.println("STOVE: Trovato STATO=O. Stufa spenta");
       digitalWrite(ledPinStove, LOW);
       valueStove = LOW;
     }
   }

     Serial.println("Connecting to: " + szHost + ":" + httpPort);
  if (!client.connect(szHost, httpPort)) {
    Serial.println("connection failed");
    return;
  }

    ///////////////////////////////////////////////////////////
  // This will send the request to the server for the FAN
  ///////////////////////////////////////////////////////////
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
    //delay(1000);
    //String response1 = client.readStringUntil('\r');
    //String response = client.readStringUntil('\r');
    //String response = "01|1|STOVE|1|1|22|25|-|-|-|2019-02-08 23:57:17";

    char f = client.read(); //gets byte from ethernet buffer
    responseFan += f; //places captured byte in readString
  }
    client.flush();
    Serial.println("Respond from FAN request:");
    Serial.println("Analizzo la risposta: " + responseFan);
    //Effettuo chiamata per convertire la risposta in array di stringhe
    //Recupero lo stato
    //char *myResponse;
    //response.toCharArray(myResponse, 100);
    char* myResponseFan = const_cast<char*>(responseFan.c_str());

    if(!strstr(myResponseFan, "FAN")>0){
      Serial.print("Not find FAN records. Exit from while loop.");
    }else{
    
       getStatoFan(myResponseFan);
    
       Serial.print("Analizzo la risposta per lo stato di STOVE: ");
       Serial.println(*arStatoFan[3]);
       if(*arStatoFan[3] == '1'){
       Serial.println("FAN: Trovato STATO=1. Ventilatore Acceso");
       digitalWrite(ledPinFan, HIGH);
       valueFan = HIGH;
       } else {
       Serial.println("FAN: Trovato STATO=O. Ventilatore spento");
       digitalWrite(ledPinFan, LOW);
       valueFan = LOW;
     }
   }

    

    
    
 

 /*------*/ 

 /*------*/
  yield();
  Serial.println("Client Disconnected");
  Serial.println("");
  Serial.println("----------------  END LOOP ------------------");
}

//Funzione che recupera l'array con i valori dell'appliance
void getStatoStove(char* myResponse){
    //char* myResponse[100]=response;
    //char *myResponse = (char*)malloc(100*sizeof(char*));
    //char **aParam = (char**)malloc(15*sizeof(char**));
    //String aParam[15];
    char *ptr;
    int i=0;

    Serial.print("getStato: risposta arrivata: ");
    Serial.println(myResponse);
    //Serial.println(response);
    //strcpy(myResponse, response);
    //Serial.print("getStato: risp. trasfor. in: ");
    //Serial.println(szPippo);
    //Serial.println(szPippo);

    ptr=strtok(myResponse, cDelim);
    //ptr=strtok(szPippo, cDelim);
    while (ptr != NULL){
      arStatoStove[i++] = ptr;
      ptr = strtok (NULL, "|");
    }

  Serial.println("\ngetStatoStove: ******* Parametri Trovati [STOVE] ********\n");
  
  for (i = 0; i < 15; ++i)  {
    Serial.println(arStatoStove[i]);  
  }

     yield();
    //return aParam;
  }

  //Funzione che recupera l'array con i valori dell'appliance
void getStatoFan(char* myResponse){
    //char* myResponse[100]=response;
    //char *myResponse = (char*)malloc(100*sizeof(char*));
    //char **aParam = (char**)malloc(15*sizeof(char**));
    //String aParam[15];
    char *ptr;
    int i=0;

    Serial.print("getStato: risposta arrivata: ");
    Serial.println(myResponse);
    //Serial.println(response);
    //strcpy(myResponse, response);
    //Serial.print("getStato: risp. trasfor. in: ");
    //Serial.println(szPippo);
    //Serial.println(szPippo);

    ptr=strtok(myResponse, cDelim);
    //ptr=strtok(szPippo, cDelim);
    while (ptr != NULL){
      arStatoFan[i++] = ptr;
      ptr = strtok (NULL, "|");
    }

  Serial.println("\ngetStatoFan: ******* Parametri Trovati [FAN] ********\n");
  
  for (i = 0; i < 15; ++i)  {
    Serial.println(arStatoFan[i]);  
  }

     yield();
    //return aParam;
  }
  
