# Arduino & NRF2401+ Drone

### What is this project about?
This is a complete **LOW COST** guide for DIY Arduino and NRF2401+ based drone package which includes: 
- Transmitter(TX)
- Receiver(RX)
- Flight Controller(FC) using [MultiWii](https://github.com/multiwii)
- Frame (the body of done) 
- How to pick some low cost parts like motors, Escs, batteries, propellers and so forth.



The aim of this project is mainly for people want try out RC Drone hobby without making too much damage to their bank account, also for educational purposes. I would rather say "learn together" as I am new to hardware and C/C++ programming.

To build a drone by following this project DOES NOT require expirence of software programming, just update the sketch onto your board and it should work, but you do need to be able to do some soldering work and some tools like soldering iron for sure. If you would like to learn some Arduino development skills, let's learn together! If you are a C/C++ programmer, it would be really appreciated if you can comment/help on improving this project, especially on my newbie's sketches. (I am a .NET developer has no background of hardware (C/C++) programming at all, sketches in this project are "working" but I am pretty sure there are many places could be better, eg. data types.)

(//TODO: some images/videos about this project)

# How Many Things
# Transmitter: 

Minimum:
 - 1 x Arduino Pro Mini 8MHZ 3.3V
 - 1 x NRF2401+
 - 2 x Joystick (a joystic is essentially 2 potentiometers)

Better range and stability:
 - 1 x NRF2401+ with PA and external antena (to replace the one above)
 - 1 x Electrolytic Capacitor 10UF
 - 1 x Ceramic Capacitor 0.1UF
 - 1 x 3.3V Voltage Step Down Module (Someone claims Pro Mini VCC output does not have enough juice for the NRF module, but my expirence it is good enough for this application)

More features: (more and more...until you run out of Arduino pins)
 - n x Push Button
 - n x Toggle Switch
 - 2 x LED
 - 1 x Buzzer
 - 1 x I2C LCD
 - 1 x Resistor 330Ω
 - 1 x Resistor 220Ω
 - n x Resistor 10KΩ

Make it sexy:
 - Find an old or trashed radio transmitter (maybe from some RC toys) and utilise its shell, most of time you also get better potentiometers (joystick) from them, at least better than the one above in my opinion.

And Lastly, you also need:
 - 1 x Prototype Board
 - Some Jump-wire
 - some Male/Female pins
 - Power source (battery)
 
