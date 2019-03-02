#include <ESP8266WiFi.h>

const char* ssid = "AP_IOCENTRO";
const char* password = "ioCentro1";

int ledPin = 13; //GPIO13

WiFiServer server(80);

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
  server.begin();
  Serial.println("Server Started");

  //Print the IP ADDRESS
  Serial.print("Use this URL to connect: http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client){
    return;
    }

  //Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    Serial.print(".");
    delay(500);
  }

  //Read the first line of the request string
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  //match the request
  int value = LOW;
  if(request.indexOf("LED=ON")!=-1){
    digitalWrite(ledPin, HIGH);
    value = HIGH;
    }
  if(request.indexOf("LED=OFF")!=-1){
    digitalWrite(ledPin, LOW);
    value = LOW;
    }

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
/*
  client.println("<br><br>");
  client.println("<a href=\"LED=ON\"><button>Turn On</button></a>");
  client.println("<a href=\"LED=OFF\"><button>Turn Off</button></a><br/>");
*/
    
  client.println("<\html>");

  delay (100);
  Serial.println("Client Disconnected");
  Serial.println("");
    
}
