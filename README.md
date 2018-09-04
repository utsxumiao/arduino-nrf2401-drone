# Arduino & NRF2401+ Drone

//TODO: some images/videos about this project
### What Is This Project About?
This is a complete **LOW COST** guide for DIY Arduino and NRF2401+ based drone package which includes: 
- [Transmitter(TX)](/transmitter)
- [Receiver(RX)](/receiver)
- [Flight Controller(FC)](/flight-controller)
- [Frame](/frame) (the body of drone) 
- [Other Parts](/other-parts) Brushless motors, Escs, batteries, propellers and so forth.

### The Motivation
I've been in RC hobby for about three years so far, mainly building and flying drones and some glider planes. I've been seeing friends, neighbours and, sometimes, boys in the park shown great interest about DIY RC hobby, most of them played toy drones and wanna go deeper into DIY. Then they are scared away or stepped back after asking how much it will cost to enter DIY RC world (they actually mean the set to fly). 

Products on shelf are normally over priced in Australia and only good for an urgent need, understandablly, labour and shipping cost is quite high here and the tiny market makes the case even worse, vendors only stock high value (of course better quality and better performance) items in order to maintain an acceptable margin. I do not against high end products as I do own some decent drone parts, transmitters and have been spending thousands over these years but that is not the topic.

I was woundering if there is any way to let those guys try out in the pool before jumping into the ocean. Until recently I dive into [MultiWii](https://github.com/multiwii) and [Arduino](https://www.arduino.cc), I realised that it is absolutely possible to build the whole lot within $50 and they are hobby grade rather than toy.

### The Goal
The aim of this project is mainly for people want try out DIY RC Drone without making too much damage to their bank account, ultimately get your DIY drone in the air under **$50**(USD). Also for learning purposes I would like to learn together on hardware and C/C++ programming as I am new to this as well. We are not going to reinvention the wheel if any aspect of this project has been done by someone, we are here to put them in use and improve it if possible.

### Prerequisites
To build a drone by following this project DOES NOT require expirence of software programming, just some simple configuration by instruction then upload the sketch onto your board and it should work, of course you do need to do some soldering work and some tools like soldering iron for those electronics parts and some work on the frame. 

### Welcome
If you would like to learn Arduino development, let's learn together! If you are a C/C++ programmer, it would be really appreciated if you can comment/help on improving this project, especially on my newbie's sketches. (I am a .NET developer have no background of hardware (C/C++) programming at all, sketches in this project are "working" but I am pretty sure there are many places could be better, eg. data types.)

### Side Notes
 - ####Radio Frequency####. The NRF24L01+ module provides 126 channels (0 - 125) mapping frequencies from 2400mhz to 2525mhz respectively, if you are planning to use it in areas close to residential like even indoor (a lot of WIFI) better stick to channels from 85 (2485mhz) and above to minimize interference, it gives you 40 channels to choose from, for simplicity I prefer starts from 100 and 25 channels is plenty for me. According to [List of WLAN channels](https://en.wikipedia.org/wiki/List_of_WLAN_channels) most WIFI routers are using frenquency under 2484mhz.

### Acknowlegements
//TODO:

### Licensing
[MIT License](/LICENSE)

