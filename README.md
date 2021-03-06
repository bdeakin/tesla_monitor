# tesla_monitor
A simple Sketch for powering an 8266 to pull down metrics from Teslascope and display on a small multi-function display. This is my first git project and my first time coding C++ since college.

![Photo of monitor](https://user-images.githubusercontent.com/38141034/91860742-0ab34300-ec3a-11ea-96a0-d84eb9528ff8.jpeg)

# Prerequisites

You need to own a Tesla. You need to signup at Teslascope.com. Once you do, you'll get a public id for your car. You'll use that public ID in the sketch itself (see comments in the provided code). You'll also get an API key for your vehicle through Teslascope, which you'll also need (see comments in code).

# Setup

There are a number of tutorials for setting up the Arduino IDE to be able to work with an 8266, just search for those.

# Materials

8266 board I used: https://www.amazon.com/gp/product/B081CSJV2V/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1

20x4 display I used: https://www.amazon.com/gp/product/B07D7ZQVDR/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1

You can get any standard breadboard and jumper cables to make this project, or you can do what I did and 3D print a case, solder it together and make a "professional"-ish looking multifunction device to display.

# Fabrication

Here's a quick tutorial on how to solder an ESP8266 to a 20x4. It also includes instructions for setting up the Arduino library for talking to the display (LiquidCrystal_I2C).

https://www.instructables.com/id/Interfacing-LCD-20X4-Display-to-Nodemcu/

# Supporting Materials

I utilized some tutorials from https://randomnerdtutorials.com to setup the 8266 to support over-the-air (OTA) updates. Here's the relevant tutorial:

https://randomnerdtutorials.com/esp8266-ota-updates-with-arduino-ide-over-the-air/


