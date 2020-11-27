/*
 WiFi OSC message host for Motivation Radio Eurorack ESP32 module
 R Heslip March 2019 

 This sketch was written for and tested with the iOS app TouchOSC but it should work with anything that sends OSC messages in the format described below.
 
 The TouchOSC app allows you to create custom iPad UIs with sliders, buttons, switches, X-Y controllers etc. When you use the UI widgets TouchOSC sends
 an OSC message to a preconfigured IP address and port. This sketch will convert OSC messages to Gate signals and CV outputs on Movitation Radio. Its
 possible to send OSC messages from Motivation Radio to TouchOSC but this requires knowing the IP address of the iPad. There is probably some easy way to do this
 (Bonjour?) but its sending OSC messages is not implemented at this point.
 
 You will have to change the ssid and password in the sketch (below) to match your WiFi network. Expressif has an API to set up SSID and password with an external app but I have not
 had much luck with it. 
 
 When Motivation Radio connects to the WiFi network via DHCP it will display its local IP on the OLED display. You have to enter this IP in the TouchOSC app
 so it knows where to send OSC messages. This sketch expects messages on port 8888 so you will have to specify that in TouchOSC as well. 
 Each of the UI "widgets" can send an OSC message which must be in the format described below. Please read the TouchOSC documentation and learn to use the TouchOSC editor!
 
 To set GateA low, send the OSC message: 
 
 GateoutA,0 

 Note: the number should be sent as a float but is treated as boolean 0 or 1. To set GateA high send the OSC message:
 
 GateoutA,1 
 
 Same message format is used  for GateoutB-D

 OSC CV messages- to set CVoutA to its lowest voltage (approx 0V) send the OSC message:
 
 CVoutA,0    
 
 The number should be sent as a float ranging from 0 (approx 0V out) to 1 (approx 6.144V out). This is intended to be used with sliders, X-Y pads etc.
 
 Same message format is used for CVoutB-D
 Incoming OSC messages are NOT checked for valid range so be careful what you send or you may not get what you expect out.
 
 History:
 March 19/19 - ported code from proto HW to Motivation Radio HW and made more consistent with the other Motivation Radio sketches.                              
*/

#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <Wire.h>
#include "SPI.h"
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "MORAD_IO.h"
#include "DAC.h"
#include "MORAD_utility.h"

#define OLED_RESET -1  // sharing with reset pin
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char ssid[] = "your_SSID";          // your network SSID (name)
char pass[] = "your_password";      // your network password

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
const IPAddress outIp(10,40,10,105);        // remote IP (not needed for receive)
const unsigned int outPort = 9999;          // remote port (not needed for receive)
const unsigned int localPort = 8888;        // local port to listen for UDP packets (here's where we send the packets)


OSCErrorCode errorcode;


void setup() {

  Serial.begin(115200);
  
  pinMode(DAC0_CS, OUTPUT);
  pinMode(DAC1_CS, OUTPUT);
  pinMode(ADC_CS, OUTPUT);  
  digitalWrite(DAC0_CS, HIGH);
  digitalWrite(DAC1_CS, HIGH);
  digitalWrite(ADC_CS, HIGH);
  pinMode(GATEout_0, OUTPUT);
  pinMode(GATEout_1, OUTPUT);
  pinMode(GATEout_2, OUTPUT);
  pinMode(GATEout_3, OUTPUT);
  
  pinMode(GATEin_0, INPUT);
  pinMode(GATEin_0, INPUT);
  pinMode(GATEin_0, INPUT);
  pinMode(GATEin_0, INPUT);

  SPI.begin(SCLK,MISO,MOSI,DAC0_CS); // we actually use a CS pin for each DAC
  SPI.setBitOrder(MSBFIRST);
  SPI.setFrequency(2000000); // ADC max clock 2 Mhz 
    
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.clearDisplay();
  display.println("   MORAD WiFiOSC");
//  display.println("    Mar 19/2019");
  display.println();
  display.display();
  delay(5000);
    
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //display.print("  IP: ");
  display.print(WiFi.localIP());
  
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  display.print(":");
#ifdef ESP32
  Serial.println(localPort);
  display.print(localPort);
#else
  Serial.println(Udp.localPort());
#endif
  display.display();
}


// Gate output message handlers - TouchOSC push buttons

void gateoutA(OSCMessage &msg) {
  float val = msg.getFloat(0); // values 0 to 1
  GATEout(0,(bool)val);
}

void gateoutB(OSCMessage &msg) {
  float val = msg.getFloat(0); // values 0 to 1
  GATEout(1,(bool)val);
}

void gateoutC(OSCMessage &msg) {
  float val = msg.getFloat(0); // values 0 to 1
  GATEout(2,(bool)val);
}

void gateoutD(OSCMessage &msg) {
  float val = msg.getFloat(0); // values 0 to 1
  GATEout(3,(bool)val);
}


void CVoutA(OSCMessage &msg) {
  float val = msg.getFloat(0); // values 0 to 1
  CVout(0,(unsigned)(val*(DAC_RANGE-1)));
}

void CVoutB(OSCMessage &msg) {
  float val = msg.getFloat(0); // values 0 to 1
  CVout(1,(unsigned)(val*(DAC_RANGE-1)));
}

void CVoutC(OSCMessage &msg) {
  float val = msg.getFloat(0); // values 0 to 1
  CVout(2,(unsigned)(val*(DAC_RANGE-1)));
}

void CVoutD(OSCMessage &msg) {
  float val = msg.getFloat(0); // values 0 to 1
  CVout(3,(unsigned)(val*(DAC_RANGE-1)));
}

void loop() {
  OSCMessage msg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError()) {
      msg.dispatch("/GateA", gateoutA);
      msg.dispatch("/GateB", gateoutB);
      msg.dispatch("/GateC", gateoutC);
      msg.dispatch("/GateD", gateoutD);
      msg.dispatch("/CVoutA", CVoutA);
      msg.dispatch("/CVoutB", CVoutB);
      msg.dispatch("/CVoutC", CVoutC);
      msg.dispatch("/CVoutD", CVoutD);
    } else {
      errorcode = msg.getError();
      Serial.print("error: ");
      Serial.println(errorcode);
    }
  }
}
