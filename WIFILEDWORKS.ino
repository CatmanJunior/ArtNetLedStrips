#include <I2Cdev.h>
#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiUdp.h>
#include <ArtnetWifi.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>

//Wifi settings----------------------------------------------------------------------------
const char* ssid = "test";
const char* password = "testtest";

// Neopixel settings-----------------------------------------------------------------------
const int numLeds = 10; // change for your setup
const int numberOfChannels = numLeds * 3; // Total number of channels you want to receive (1 led = 3 channels)

const byte dataPin = 15; //Pin for DataIn of Leds(D2 on ESP is D4 WEMO D1 MINI/D9 on WemosD1R1)
const float brightness = 0.7; //Brightness (0-1)

Adafruit_NeoPixel leds = Adafruit_NeoPixel(numLeds, dataPin, NEO_GRB + NEO_KHZ800);

// Artnet settings------------------------------------------------------------------------
ArtnetWifi artnet;
const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.



//DEBUGMODE
bool debugOn = false;
byte debugpin = 4; //3 on Wemo D1 Mini

// connect to wifi – returns true if successful or false if not
boolean ConnectWifi(void)
{
  boolean state = true;
  int i = 0;

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");
  //Yellow in connexion mode
  for (int i = 0 ; i < numLeds ; i++) {
    leds.setPixelColor(i, 127, 127, 0);
  }
  leds.show();
  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    leds.setPixelColor(i, 255, 0, 0); //Turn pixels red while trying
    leds.show();
    delay(500);
    Serial.print(".");
    if (i > 20) {
      state = false;
      break;
    }
    i++;
  }

  if (state) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    //Green if succes
    for (int i = 0 ; i < numLeds ; i++) {
      leds.setPixelColor(i, 0, 255, 0);
      leds.show();
      delay(100);
    }
    delay(500);
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
  }

  return state;
}

void initTest()
{
  //Test strip sequence
  //Build up white
  for (int i = 0 ; i < numLeds ; i++) {
    leds.setPixelColor(i, 127, 127, 127);
    leds.show();
    delay(100);
  }
  //Red
  for (int i = 0 ; i < numLeds ; i++) {
    leds.setPixelColor(i, 255, 0, 0);
  }
  leds.show();
  delay(333);
  //Green
  for (int i = 0 ; i < numLeds ; i++) {
    leds.setPixelColor(i, 0, 255, 0);
  }
  leds.show();
  delay(333);
  //Blue
  for (int i = 0 ; i < numLeds ; i++) {
    leds.setPixelColor(i, 0, 0, 255);
  }
  leds.show();
  delay(333);
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{
  if (universe != startUniverse) return; //Don't process other universes

  // read universe and put into the right part of the display buffer
  for (int i = 0; i < length / 3; i++)
  {
    if (i < numLeds)
      if (universe == startUniverse)
        leds.setPixelColor(i, data[i * 3], data[i * 3 + 1] , data[i * 3 + 2] );
  }
  leds.show();
}

void DebugMode()
{
  for (int i = 0 ; i < numLeds ; i++) {
    leds.setPixelColor(i, 0, 0, 255);
  }
  leds.show();
  ArduinoOTA.handle();
}

void setup()
{
  Serial.begin(115200);
  pinMode(debugpin, INPUT);
  ArduinoOTA.begin();
  leds.begin();
  leds.setBrightness(brightness * 255);

  initTest();
  //Blink red if no wifi, then reset
  if (ConnectWifi() == false) {
    for (int j = 0; j < 5; j++) {
      for (int i = 0 ; i < numLeds ; i++) {
        leds.setPixelColor(i, 255, 0, 0);
      }
      leds.show();
      delay(333);
      for (int i = 0 ; i < numLeds ; i++) {
        leds.setPixelColor(i, 0, 0, 0);
      }
      leds.show();
      delay(333);
    }
    ESP.reset();
  }

  artnet.begin();
  // this will be called for each packet received
  artnet.setArtDmxCallback(onDmxFrame);
}

void loop()
{
  //  int buttonState = digitalRead(debugpin);
  //  if (digitalRead(debugpin) == LOW) {
  //    debugOn = !debugOn;
  //    delay(200);
  //  }
  //
  //  Serial.println("mode: ");
  //  Serial.print(debugOn);
  //  Serial.println("button ");
  //  Serial.print(buttonState);
  //  if (debugOn)
  //  {
  //    DebugMode();
  //  } else {
  // we call the read function inside the loop
  artnet.read();
      delay(1); //Stability delay
//}
}
