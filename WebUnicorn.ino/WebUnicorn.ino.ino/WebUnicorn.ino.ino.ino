#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>

int inbuild_ledPin = 16;
bool ledState = LOW;
ESP8266WebServer server(80);

void setup() {
pinMode(ledPin, OUTPUT);
Serial.begin(115200);
WiFi.begin("NewMediaDevNet", "BourbonFreeman"); //Connect to the WiFi network
while (WiFi.status() != WL_CONNECTED) { //Wait for connection
delay(500);
Serial.println("Waiting to connect…");
}

Serial.print("Connect on http://");
Serial.println(WiFi.localIP()); //Print the local IP

server.on("/on", turnOn);         //Associate the handler function to the path
server.on("/off", turnOff);        //Associate the handler function to the path
server.on("/toggle", toggle);   //Associate the handler function to the path

server.begin(); //Start the server
Serial.println("Server listening");

}

void loop() {
  server.handleClient();
}

void turnOn(){
ledState = LOW;
digitalWrite(inbuild_ledPin, ledState);
server.send(200, "text/plain", "LED on");
}

void turnOff(){
ledState = HIGH;
digitalWrite(inbuild_ledPin, ledState);
server.send(200, "text/plain", "LED off");
}

void toggle(){
ledState = !ledState;
digitalWrite(inbuild_ledPin, ledState);
server.send(200, "text/plain", "LED toggled");
}

//*****************Start FastLED Functions****************

