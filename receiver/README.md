# Transmitter: 
Depends on which of following set you choose, this transmitter can be a sort of hobby grade transmitter which capable to cover 1km (2km according to NRF24L01+ PA version datasheet) area and can bind to multiple receivers. If you have/can find a trashed hooby RC transmitter I suggest to use it as the shell of our curcuit board as they normally looked better and also we can utilise joysticks in it most of the time.
//TODO: image or video

### Tools & Materials required:
- Soldering iron
- Some solder wire
- Screw driver (if you decide to use a toy transmitter as the housing)
- Cutter knife (AS ABOVE and to use the joysticks in it)
- Multimeter (optional, but recommaded)
- Hot glue gun (optional)

### Option Minimum:
Components | Cost | Qty | Optional
-----------|------|-----|---------
Arduino Pro Mini (recommend 3.3V version)|$1|1|No
NRF24L01+|$1|1|No
Joystick|$1|2|Yes/No. Can be replaced by a toy transimitter
LED|$0|1|Yes. Cost almost nothing
Electric Wire|$1|several|Let's say $1 for LED and some electric wires

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
 
