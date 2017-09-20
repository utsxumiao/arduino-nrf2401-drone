# Arduino & NRF2401+ Drone
This is a complete solution for Arduino and NRF2401+ based drone solution. From transmitter to receiver then flight controller (Multiwii).

(//TODO: details of optional and resistor values)
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
 
