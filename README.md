# pan-biosphere
Particle photon firmware for automating my aquarium. Includes a full LCD GUI and an RGB rotary encoder. Linked to Blynk, and partially cloud accessible, but my goal is to further incorporate Blynk into the project to make the whole thing cloud accessible.

This is by no means a plug and play system yet, so you'll probably have to do a bit of adapting to get it to work for you. My vision is to create something neat and easily adaptable but this is more of a proof of concept/alpha prototype towards that.
That being said it could be worthwhile; I think anyone wishing to duplicate the project will find it useful for their aquarium. The emphasis of this project was the UX and the practicality of quickly interfacing with the aquarium.

Important note!

I'm not adding the libraries in this repository. Most of them are on the Particle web IDE. The only one that isn't is the I2C lcd library, but you can find all of the neccesary code here: [gist](https://gist.github.com/anonymous/dc7ecc5e6574105b4fbd) or at the link to the tutorial [below](https://github.com/eriknakamura/pan-biosphere/blob/master/README.md#resources). 

# Components

Main Components
1. Particle Photon
2. Serial 20x4 I2C LCD
3. DHT11 or DHT22 humidity probe (currently running a DHT11 but DHT22 switch will come soon)
4. Waterproof Dallas DS18B20 temp. probe (currently do not have this working)
5. Sparkfun RGB Rotary Encoder
6. Relay Shield configured to NC and open on LOW

Extra Components
1. 2 x .1 mico farad capacitor
2. 1 x 4k7 Ohm resistor
3. 1 x 150 Ohm resistor
4. 2 x 100 Ohm resistor
5. 1 x 10k Ohm resistor
6. A bunch of wire
7. Whatever else you need to get it to work :)

# Pinouts
This is just data lines, Vin and Ground is pretty self explanatory and varies per device. I might have missed some, confirm in code. 
- LCD SDA -- D0
- LCD SCL -- D1
- RGB Rotary Encoder Button -- D3
- RGB Rotary Encoder A -- D4
- RGB Rotary Encoder B -- D5
- Relay One -- D6
- Relay Two -- D7
- DHTXX -- A0
- RGB Rotary Encoder Red -- A4
- RGB Rotary Encoder Green -- A7
- RGB Rotary Encoder Blue -- A5


# Resources

1. [How to use the I2C LCD Library and set up the LCD](https://community.particle.io/t/lcd-i2c-lib-info-customcharacters/4670)
2. [How to wire up the RGB Rotary Encoder](http://blue-pc.net/2014/07/25/rotary-encoder-mit-rgb-led/)
3. [Blynk](http://docs.blynk.cc/)

Libraries Used
1. Blynk (In web IDE)
2. Adafruit DHT (In web IDE)
3. rgb-controls (In web IDE)
4. OneWire (In web IDE) Haven't actually gotten the DS18B20 to work yet
5. I2C LCD Library from [this Particle forum](https://community.particle.io/t/lcd-i2c-lib-info-customcharacters/4670)

Sources of Inspiration

  There is lots of code I just scraped off of random forums and places on the internet. Here are the sources I could remember/find again. 
 1. [A google doc someone made about rotary encoder LCD interfaces](https://drive.google.com/drive/u/0/folders/0B5m_NdW7qSxgdkV4R1RNYzE0Rnc)
 2. [Some code for the rotary encoder from this Particle forum](https://community.particle.io/t/rotary-encoder-basics/6135)
 
