#include <ESP8266WiFi.h>

//const char* ssid = "AP_IOCENTRO";
//const char* password = "ioCentro1";
const char* ssid = "BenMiA2";
const char* password = "benedetto";
const int httpPort = 80;


//String szHost="http://192.168.23.120";
String szHost="192.168.43.141";

//const char* szHost="www.google.com";
//String szHost="www.google.com";

String urlReq01="/iot/select_from_db.php?id_req=01";
String urlReq02="/iot/select_from_db.php?id_req=02";

String response="";

int ledPin = 13; //GPIO13
int value;

WiFiServer server(80);
WiFiClient client;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  //Connect to Wi Fi Network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    }
  Serial.println("WiFi Connected");

  //Start the server
  //server.begin();
  //Serial.println("Server Started");

  //Print the IP ADDRESS
  //Serial.print("Use this URL to connect: http://");
  //Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  // Check if a client has connected
  //WiFiClient client = server.available();
  //if (!client){
  //  return;
  //  }

  delay (1000);

   Serial.println("Connecting to: " + szHost + ":" + httpPort);
  if (!client.connect(szHost, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  //--------------
  // We now create a URI for the request
  //String url = "/stan";

  //Serial.print("Requesting URL: ");
  //Serial.println(url);

  // This will send the request to the server
  Serial.println("Invio della richiesta");
  Serial.println(String("GET ") + urlReq01 + " HTTP/1.1\r\n" +
               "Host: " + szHost + "\r\n" + 
               "Connection: close\r\n\r\n");
  
  client.print(String("GET ") + urlReq01 + " HTTP/1.1\r\n" +
               "Host: " + szHost + "\r\n" + 
               "Connection: close\r\n\r\n");
  
  
  //client.print(String("GET ") + urlReq01);
  delay(100);

  // Read all the lines of the reply from server and print them to Serial
  Serial.println("Respond:");
  while(client.available()){
    response = client.readStringUntil('\r');
    Serial.print(response);

      Serial.print("Analizzo la risposta: " + response);
  if(response.indexOf("STATO=1")!=-1){
    Serial.print("Trovato STATO=1. Luce Accesa");
    digitalWrite(ledPin, HIGH);
    value = HIGH;
    }
  if(response.indexOf("STATO=0")!=-1){
    Serial.print("Trovato STATO=O. Luce spenta");
    digitalWrite(ledPin, LOW);
    value = LOW;
    }
  }

  //--------------
  
  //client.flush();

  //match the request
  //int value = LOW;
  /*
  Serial.print("Analizzo la risposta: " + response);
  if(response.indexOf("STATO=1")!=-1){
    Serial.print("Trovato STATO=1. Luce Accesa");
    digitalWrite(ledPin, HIGH);
    value = HIGH;
    }
  if(response.indexOf("STATO=0")!=-1){
    Serial.print("Trovato STATO=O. Luce spenta");
    digitalWrite(ledPin, LOW);
    value = LOW;
    }
    */

  client.flush();

/*
  //Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.print("Led Pin is now: ");

  if(value == HIGH){
    client.print("ON");
  } else {
    client.println("OFF");  
  }

  client.println("<br><br>");
  client.println("<a href=\"update_db_state.php?nuovo_stato=ON\"><button>Turn On</button></a>");
  client.println("<a href=\"update_db_state.php?nuovo_stato=OFF\"><button>Turn Off</button></a><br/>");
  */
/*
  client.println("<br><br>");
  client.println("<a href=\"LED=ON\"><button>Turn On</button></a>");
  client.println("<a href=\"LED=OFF\"><button>Turn Off</button></a><br/>");
*/
/*
  client.println("<\html>");
*/
  
  Serial.println("Client Disconnected");
  Serial.println("");
    
}
