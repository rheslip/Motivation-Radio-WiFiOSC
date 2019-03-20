### WiFi OSC Message Host the Motivation Radio Eurorack module

Motivation Radio is a general purpose eurorack module based on the ESP32 with:

* 4 Gate/Trigger inputs

* 4 CV inputs -5V to +5V range, 12 bit resolution

* 4 Gate/Trigger outputs 0-6V

* 4 CV outputs 0-6.144V 12 bit resolution

* 128x32 OLED display

* Encoder with switch

* MIDI in and out with 1/8 jacks - MIDI in works with either polarity (Korg or Arturia), MIDI out is MMA standard (Korg)

* Bluetooth and Wifi are supported by the ESP32 module


Motivation Radio is intended mainly for interfacing eurorack to other devices (e.g. iPad) wirelessly but it can do more!

### Hardware Usage

* Gate inputs - currently unused in this sketch

* CV inputs - currently unused in this sketch

* Gate outputs - controlled by OSC messages

* CV outputs - voltages set by OSC messages


Serial MIDI in - currently unused in this sketch

Serial MIDI out - currently unused in this sketch

Encoder and Button - currently unused in this sketch



### How to use WiFiOSC

This sketch was written for and tested with the iOS app TouchOSC https://hexler.net/software/touchosc but it should work with anything that sends OSC messages in the format described below.
 
The TouchOSC app allows you to create custom iPad UIs with sliders, buttons, switches, X-Y controllers etc. When you use the UI widgets TouchOSC sends an OSC message to a preconfigured IP address and port. This sketch will convert OSC messages to Gate signals and CV outputs on Movitation Radio. Its possible to send OSC messages from Motivation Radio to TouchOSC but this requires knowing the IP address of the iPad. There is probably some easy way to do this (Bonjour?) but sending OSC messages is not implemented at this point.
 
You will have to change the ssid and password in the sketch to match your WiFi network. Expressif has an API to set up SSID and password using an external app but I have not had much luck with it. 
 
When Motivation Radio connects to the WiFi network via DHCP it will display its local IP on the OLED display. You have to enter this IP in the TouchOSC app so it knows where to send OSC messages. This sketch expects messages on port 8888 so you will have to specify that in TouchOSC as well. Each of the UI "widgets" can send an OSC message which must be in the format described below. Please read the TouchOSC documentation and learn to use the TouchOSC editor!
 
To set Gate Out A (leftmost on the Motivation Radio panel) low, send the OSC message: 
 
/GateoutA,0 

Note: the number should be sent as a float but is treated as boolean 0 or 1. To set Gate Out A high send the OSC message:
 
/GateoutA,1 
 
The same message format is used  for Gate outs B, C and D

 OSC CV messages- to set CV Out A to its lowest voltage (approx 0V) send the OSC message:
 
/CVoutA,0    
 
The number should be sent as a float ranging from 0 (approx 0V out) to 1 (approx 6.144V out). This range is intended to be used with slider widjets, X-Y pads etc. 
 
The same message format is used for CV outs B, C and D

Incoming OSC messages are NOT checked for valid range so be careful what you send or you may not get what you expect out.
 
A very simple UI for TouchOSC is included in /resources. Four buttons control gates A-D and four sliders adjust the CV outputs A-D. You will have to load this template in the TouchOSC editor on a PC and sync it to the TouchOSC app on your iPad.


### Software Dependendencies:

* Adafruit Graphics library and the SSD1306 driver https://github.com/adafruit/Adafruit-GFX-Library https://github.com/adafruit/Adafruit_SSD1306

* OSC for Arduino https://github.com/CNMAT/OSC


compiled with Arduino 1.85 with ESP32 Arduino installed. Use "ESP32 DEV Module" as the board. 

More info in the Wiki: https://github.com/rheslip/Motivation-Radio---WiFiOSC/wiki

Rich Heslip March 2019

