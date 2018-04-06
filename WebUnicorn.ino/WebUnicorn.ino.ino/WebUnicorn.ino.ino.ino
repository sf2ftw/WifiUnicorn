#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FastLED.h>

int inbuild_ledPin = 16;
bool ledState = LOW;
bool previousState = LOW;

ESP8266WebServer server(80);
#define LED_PIN     2
#define NUM_LEDS    3
#define BRIGHTNESS  96
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100

CRGBPalette16 currentPalette( CloudColors_p);
CRGBPalette16 targetPalette( RainbowColors_p);

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
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  server.on("/on", turnOn);         //Associate the handler function to the path
  server.on("/off", turnOff);        //Associate the handler function to the path
  server.on("/toggle", toggle);   //Associate the handler function to the path
  
  server.begin(); //Start the server
  Serial.println("Server listening");

}

void loop() {
  previousState = ledState;
  server.handleClient();
  ///****need to thread this******
  ChangePalettePeriodically();
  uint8_t maxChanges = 24; 
  nblendPaletteTowardPalette( currentPalette, targetPalette, maxChanges);
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */
  FillLEDsFromPaletteColors( startIndex);
 /*if (ledState = 1) {
  if (previousState = LOW) { 
       turnLEDSOn();
       Serial.println("Turning on LEDS");
   }
 } else {
  if (previousState = HIGH) {
    turnLEDSOff(); 
    Serial.println("Turning off LEDS");
  }
 }*/
  //*******end of fastled stuff************
}

void turnOn(){
ledState = LOW;
turnLEDSOn();
digitalWrite(inbuild_ledPin, ledState);
server.send(200, "text/plain", "LED on");
}

void turnOff(){
ledState = HIGH;
turnLEDSOff();
digitalWrite(inbuild_ledPin, ledState);
server.send(200, "text/plain", "LED off");
}

void toggle(){
ledState = !ledState;
digitalWrite(inbuild_ledPin, ledState);
server.send(200, "text/plain", "LED toggled");
}

void turnLEDSOn(){
  Serial.println("Turning on LEDS");
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}
void turnLEDSOff(){
  
 FastLED.clear();
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

