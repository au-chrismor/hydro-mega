# Hydroponic Controller

## Introduction
This code shows how to control an "Ebb and Flow" hydroponic installation using an Arduino Mega controller.

Ebb and Flow is a very simple process where nutrient is sprayed into the growth medium and allowed to drain back to the main reservoir.  A typical duty cycle is 2 hours flood, followed by 15 minutes drain.

Timing does not require anything in the way of precision (within a minute is fine), so we simple accumulate time with delay() calls.  If this is a problem, and perhaps it will be for future functions, we could add some real-time device to get us better control, but for now it just is not justified.

## Requirements
+ Arduino Mega 2560 (if debug information is removed, it is possible to fit into a Uno, but it's very tight for memory)
+ WiFi or Ethernet Shield (or equivalent built-in functionality)
+ DHT11 or DHT22 sensor
+ Dallas 18B20 sensor in waterproof housing (also a 4K7 resistor)
+ Homebrew Ec probe (See References)
+ Relay Module(s)

## Compiling
First, decide which features you wish to use, because there are dependencies.  For example, you can't use ThingSpeak without a network connection, and you can't measure Ec without the 18B20 sensor.  Enabling and disabling a feature is a matter of updating sysconfig.h

**DO NOT** enable multiple network interfaces, as it won't compile (for now).  GSM is a stub only for now, and when I get my hands on some proper hardware I will give it the attention it deserves.

## Known Issues
+ You can only have a single network technology enabled.  Later I will get smart and figure out how to deal with better.  But for now, just don't do it.
+ GSM support is non-existent at the moment.
+ All temperatures are given in Centigrade.  It would be easy enough to change this.
+ Only a single DS18B20 device is supported.  If you have more than one installed, only the lowest ID device will be used, and this might not be the device you were expecting.

## Help Wanted.
If you'd like to help out, drop me a line via GitHub.  Or just fork the repo

## References
https://hackaday.io/project/7008-fly-wars-a-hackers-solution-to-world-hunger/log/24646-three-dollar-ec-ppm-meter-arduino

