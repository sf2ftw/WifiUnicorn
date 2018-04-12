#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
int inbuild_ledPin = 16;
bool ledState = 1;
bool previousState = LOW;
ESP8266WebServer server(80);

#define LED_PIN     2
#define NUM_LEDS    3

#define BRIGHTNESS  200  //default was 96
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
//#define UPDATES_PER_SECOND 100
#define UPDATES_PER_SECOND 20

NTPClient timeClient(ntpUDP);
CRGBPalette16 currentPalette( CloudColors_p);
CRGBPalette16 targetPalette( RainbowColors_p);

//***********************Start Web Functions******************************

void handleRoot(){ 
  if ( server.hasArg("on") ) {
    turnOn();
    server.send ( 200, "text/html", getPage() );
  } else if ( server.hasArg("off") ) {
    turnOff();
    server.send ( 200, "text/html", getPage() );
  } else if ( server.hasArg("toggle") ) {
    toggle();
    server.send ( 200, "text/html", getPage() );
  } else {
    server.send ( 200, "text/html", getPage() );
  }  
}

String getPage(){
  String page = "<html lang='fr'><head><meta http-equiv='refresh' content='60' name='viewport' content='width=device-width, initial-scale=1'/>";
  page += "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'><script src='https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js'></script><script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script>";
  page += "<title>Wifi Unicorn Website</title></head><body>";
  page += "<div class='container-fluid'>";
  page +=   "<div class='row'>";
  page +=     "<div class='col-md-12'>";
  page +=       "<h1>Wifi Unicorn</h1>";
  page +=       "<br>";
  page +=        "<span class='badge'>";
  if (ledState == 0) {
    page += "Off"; 
  } else {
    page += "On"; 
  }
  page +=         "</span></h4></div><br>";
  page +=          "<div class='col-md-4'><form action='/' method='POST'><button type='button submit' name='toggle' value='1' class='btn btn-success btn-lg'>TOGGLE</button></form></div>";
  page += "</body></html>";
  return page;
}

//***************************end of web page functions***************************

void setup() {
  pinMode(inbuild_ledPin, OUTPUT);
  Serial.begin(115200);
  //WiFi.begin("NewMediaDevNet", "BourbonFreeman"); //Connect to the WiFi network
  WiFi.begin("VM1709525", "qkdg9nsXmtk7"); 
  while (WiFi.status() != WL_CONNECTED) { //Wait for connection
      delay(3000);
      Serial.println("Waiting to connect…");
  }
  Serial.print("Connect on http://");
  Serial.println(WiFi.localIP()); //Print the local IP
  delay(3000); //power up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );  //add in the LEDs to fast LED
  FastLED.setBrightness(  BRIGHTNESS );
  //server.on("/on", turnOn);         //Associate the handler function to the path
  //server.on("/off", turnOff);        //Associate the handler function to the path
  //server.on("/toggle", toggle);   //Associate the handler function to the path
  server.on ("/", handleRoot); //Associate the handleRoot function to deal with all web requests
  server.begin(); //Start the server
  Serial.println("Server listening");
  timeClient.begin();
  timeClient.setTimeOffset(3600);  //need to be more clever with this at some point, summertime nonsense
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
}

void loop() {
  server.handleClient();
  if (ledState == 1) {
      turnOn();
  } else {
    turnOff();
  }
}

void turnOn(){
ledState = 1;
turnLEDSOn();
digitalWrite(inbuild_ledPin, ledState);
//server.send(200, "text/plain", "LED on");
}

void turnOff(){
ledState = 0;
turnLEDSOff();
Serial.println("ledState changed to ");
Serial.println(ledState);
digitalWrite(inbuild_ledPin, ledState);
//server.send(200, "text/plain", "LED off");
}

void toggle(){
ledState = !ledState;
if (ledState == 0) {
  turnOff();
} else {
  digitalWrite(inbuild_ledPin, ledState);
//  server.send(200, "text/plain", "LED toggled");
}
}

void turnLEDSOn(){
  Serial.println("Turning on LEDS");
  ChangePalettePeriodically();
  uint8_t maxChanges = 24; 
  //uint8_t maxChanges = 5; 
  nblendPaletteTowardPalette( currentPalette, targetPalette, maxChanges);
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */
  FillLEDsFromPaletteColors( startIndex);
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void turnLEDSOff(){
  for( int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB::Black;
     }
     FastLED.show();
}

//*****************Start FastLED Functions****************

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
  uint8_t brightness = 255;
  for( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex + sin8(i*16), brightness);
    colorIndex += 3;
  }
}

void ChangePalettePeriodically()
{
  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;
  if( lastSecond != secondHand) {
    lastSecond = secondHand;
    CRGB p = CHSV( HUE_PURPLE, 255, 255);
    CRGB g = CHSV( HUE_GREEN, 255, 255);
    CRGB b = CRGB::Black;
    CRGB w = CRGB::White;
    if( secondHand ==  0)  { targetPalette = RainbowColors_p; }
    if( secondHand == 10)  { targetPalette = CRGBPalette16( g,g,b,b, p,p,b,b, g,g,b,b, p,p,b,b); }
    if( secondHand == 20)  { targetPalette = CRGBPalette16( b,b,b,w, b,b,b,w, b,b,b,w, b,b,b,w); }
    if( secondHand == 30)  { targetPalette = LavaColors_p; }
    if( secondHand == 40)  { targetPalette = CloudColors_p; }
    if( secondHand == 50)  { targetPalette = PartyColors_p; }
  }
}
//*******************End of LED functions*********************
