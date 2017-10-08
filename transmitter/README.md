# Transmitter: 
Depends on which of following set you choose, this transmitter can be a sort of hobby grade transmitter which capable to cover 1km (2km according to NRF24L01+ PA version datasheet) area and can bind to multiple receivers. If you have/can find a trashed hooby RC transmitter I suggest to use it as the shell of our curcuit board as they normally looked better and also we can utilise joysticks in it most of the time.
//TODO: image or video

### Tools & Materials required:
- Soldering iron
- Some solder wire
- Screw driver (if you decide to use a toy transmitter as the housing)
- Cutter knife (AS ABOVE and to use the joysticks in it)
- Multimeter (optional, but recommaded)
- Hot glue gun with glue stick(optional)
- Some electric wire

### Options
#### Minimum:
Components | Cost | Qty | Optional
-----------|------|-----|---------
Arduino Pro Mini (recommend 3.3V version)|$1|1|No
NRF24L01+|$1|1|No. Covers 100m area according to datasheet
Joystick|$1|2|No. Unless you have a toy/trashed transimitter
Prototype Board|$1|1|No. Unless you have a toy/trashed transimitter
LED|$0|>=1|No. Cost almost nothing
Switch|$0|1|No. Cost see below
Push Buttons|$1|>=2|No. Let's say $1 for LED and some push buttons

#### Better Range & Stability:
Components | Cost | Qty | Optional
-----------|------|-----|---------
Arduino Pro Mini (recommend 3.3V version)|$1|1|No
NRF2401+ with PA and external antena|$5|1|No. Extend signal to 2km area according to datasheet
Joystick|$1|2|No. Unless you have a toy/trashed transimitter
Prototype Board|$1|1|NNo. Unless you have a toy/trashed transimitter
LED|$0|>=1|No. Cost almost nothing
Switch|$0|1|No. Cost see below
Push Buttons|$1|>=2|No. Let's say $1 for LED and some push buttons
Electrolytic Capacitor 10UF||1|Yes. To stablise power supply to NRF module
Ceramic Capacitor 0.1UF||1|Yes. To eliminate noise from power supply to NRF module
ASM1117 3.3v Votage Regulator||1|Yes. Separate power supply from battery to NRF module

*Someone claims Arduino Pro Mini(3.3v) VCC output does not have enough juice for the NRF module with PA, so the optional ASM1117 chip is to make separated power supply from battery to NRF module, but in my expirence the Arduino VCC works just fine for this application.

#### More Features:
Components | Cost | Qty | Optional
-----------|------|-----|---------
Arduino Pro Mini (recommend 3.3V version)|$1|1|No
NRF2401+ with PA and external antena|$5|1|No
Joystick|$1|2|Yes/No. Can be replaced by a toy transimitter
Prototype Board|$1|1|No. Unless you have a toy/trashed transimitter
LED|$0|>=1|No. Cost almost nothing
Switch|$0|1|No. Cost see below
Push Buttons|$1|>=2|No. Let's say $1 for LED and some push buttons
Electrolytic Capacitor 10UF||1|Yes. To stablise power supply to NRF module
Ceramic Capacitor 0.1UF||1|Yes. To eliminate noise from power supply to NRF module
ASM1117 3.3v Votage Regulator||1|Yes. Separate power supply from battery to NRF module
Piezo Speaker 3.3v||1|Yes. Give some sound feedback
I2C LCD|$3|1|Yes. Add a screen to our transmitter

*There are a lot more possibilities to add onto this project but it will definitely goes beyound the purpose of this project.

I strongly recommend to find a old/broken/trashed/abandoned transmitter and use it as the housing and if possbile also utilise the joysticks (most of time it has better quality), even button/switch inside. It also make the final product looks better and easier to hold in hand. If not possible, you might want to build a case for it.

Lastly, you will need some sort of power supply which needs to be above 3.3v all the time, it can be 3 to 4 AA or AAA batteries, I recommend 4 AA batteries so we can squeeze them until they goes down to 0.7v (4 cells 3.2v still OK to our application).
 
