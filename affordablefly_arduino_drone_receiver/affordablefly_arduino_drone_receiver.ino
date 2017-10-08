//********************************************************************************************************************//
//
//
//
//********************************************************************************************************************//


#include <EEPROM.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Bounce2.h>

//#define debugMode
#define clockMultiplier 1 // IMPORTANT: Set this to 2 if you are using a 16MHz arduino, leave as 1 for an 8MHz arduino


///////////////////////////////////////////// PIN Assignment ///////////////////////////////////////////////////////////
//
// I tried to avoid using pin 3, 5, 6, 9, 10 for any other non-pwm purposes, save them for PWM output later
// But if you sure about this, feel free to add more features (eg. telemetry) with rest of pins
// Becareful A6, A7 are analog input only, no internal pullup either according to official datasheet
//
// 2 => PPM Signal (connects to PPM input of flight controller)
// A0 => Channel button (connects to button -> GND)
// 4 => LED (For 3.3v Arduino: connects to LED -> GND, for 5v Arduino: connects to LED -> 330ohm resistor -> GND)
// 7, 8, 11, 12, 13 => NRF24L01+ module (7 => CE, 8 => CSN, 11 => MOSI, 12 => MISO, 13 => SCK)
#define ppmPin 2
#define channelButtonPin A0
#define ledPin 4


///////////////////////////////////////////// Variable & Type Declaration ///////////////////////////////////////////////
RF24 radio(7, 8);
const uint64_t pipeIn =  0xE8E8F0F0E1LL;

const int channelCount = 8; //Adjust to the number of channels for your application
int ppm[channelCount];

int channelAddressEEPROM = 0;
int uniqueIdAddressEEPROM = 1;
//NRF24L01+ has 126 (0-125) channels, from 110 (2510MHZ) and up we got good chance to avoid wildwest of 2.4G waves, like WIFI.
const int channelLowerBoundery = 100;
const int channelUpperBoundery = 125;
int channel = channelLowerBoundery;
byte uniqueId = 0;

const long PPM_FrLen = 27000;
const int PPM_PulseLen = 400;

bool hasSignal = false;
const long signalLostTolerance = 1000; //in ms
unsigned long lastSignalMillis = 0;

int ledState = LOW;
const long ledFlashInterval = 40;
unsigned long previousLedMillis = 0;

const long votageReadingInterval = 5000;
unsigned long previousVotageReadingMillis = 0;

const long radioBindingInterval = 50;
unsigned long previousRadioBindingMillis = 0;

Bounce channelButtonBounce = Bounce();
bool channelButtonPressed = false;

//This is data structure received from transmitter, should be same on both sides
struct ComData {
  byte uniqueId;
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
  byte AUX1;
  byte AUX2;
  byte AUX3;
  byte AUX4;
};
ComData comData;

//This is acknowledgement structure will send to transmitter, should be same on both sides
struct AckData {
  float votage;
};
AckData ackData;

struct BindingData {
  byte mode;
  byte channel;
  byte uniqueId;
};
BindingData bindingData;
BindingData ackBindingData;

enum RadioMode {
  normal,
  bind
};
RadioMode radioMode = normal;


///////////////////////////////////////////// System Initialisation ///////////////////////////////////////////////////
void setup() {
#if defined(debugMode)
  Serial.begin(115200);
  Serial.println("System starting...");
#endif
  setupButtons();
  setupLeds();
  setupRadio();

  channelButtonBounce.update();
  if (channelButtonBounce.read() == LOW) {
    radioMode = bind;
  } else {
    radioMode = normal;
    setupPPM();
    setupPWM();
    resetComData();
    resetAckData();
    getChannel();
    getUniqueId();
    radio.setChannel(channel);
    radio.startListening();
  }
#if defined(debugMode)
  Serial.print("Radio mode: ");     Serial.print(radioMode);    Serial.print("\n");
  Serial.println("System is ready.");
#endif
}


///////////////////////////////////////////// System Main Loop ///////////////////////////////////////////////////////
void loop() {
  unsigned long currentMillis = millis();
  flashLed(currentMillis);
  if (radioMode == normal) {
    readBatteryVotage(currentMillis);
    receiveData(currentMillis);
    checkSignal(currentMillis);
  } else if (radioMode == bind) {
    bindRadio(currentMillis);
  } else {
    //unknow mode, do nothing.
  }
}


///////////////////////////////////////////// Initialisation Functions ///////////////////////////////////////////////
void setupLeds() {
#if defined(debugMode)
  Serial.println("Initialising LEDs...");
#endif
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void setupButtons() {
#if defined(debugMode)
  Serial.println("Initialising buttons...");
#endif
  pinMode(channelButtonPin, INPUT);
  digitalWrite(channelButtonPin, INPUT_PULLUP);
  channelButtonBounce.attach(channelButtonPin);
  channelButtonBounce.interval(5);
}

void setupPPM() {
#if defined(debugMode)
  Serial.println("Initialising PPM output...");
#endif
  pinMode(ppmPin, OUTPUT);
  digitalWrite(ppmPin, LOW);  //set the PPM signal pin to the default state (off)
  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;
  OCR1A = 100;  // compare match register (not very important, sets the timeout for the first interrupt)
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();
}

void setupPWM() {
#if defined(debugMode)
  Serial.println("Initialising PWM output...");
#endif
  //TODO: make this receiver compatible with PWM outputs
}

void resetComData() {
#if defined(debugMode)
  Serial.println("Resetting ComData...");
#endif
  comData.throttle = 0;
  comData.yaw = 127;
  comData.pitch = 127;
  comData.roll = 127;
  comData.AUX1 = 0;
  comData.AUX2 = 0;
  comData.AUX3 = 0;
  comData.AUX4 = 0;
  setPPMValuesFromData();
}

void resetAckData() {
#if defined(debugMode)
  Serial.println("Resetting AckData...");
#endif
  ackData.votage = 0;
}

void setupRadio() {
#if defined(debugMode)
  Serial.println("Initialising radio...");
#endif
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.openReadingPipe(1, pipeIn);
}


///////////////////////////////////////////// Main Functions /////////////////////////////////////////////////////////
void receiveData(unsigned long currentMillis) {
  while (radio.available()) {
    radio.read(&comData, sizeof(ComData));
    if (comData.uniqueId == uniqueId) {
      radio.writeAckPayload(1, &ackData, sizeof(AckData));
      lastSignalMillis = currentMillis;
      hasSignal = true;
      setPPMValuesFromData();
#if defined(debugMode)
      dumpDataToSerial();
#endif
    }
  }
}

void checkSignal(unsigned long currentMillis) {
  if (currentMillis - lastSignalMillis >= signalLostTolerance) {
    hasSignal = false;
    resetComData();
#if defined(debugMode)
    Serial.print("Signal status: "); Serial.print(hasSignal); Serial.print("\n");
#endif
  }
}

void readBatteryVotage(unsigned long currentMillis) {
  if (currentMillis - previousVotageReadingMillis >= votageReadingInterval) {
    previousVotageReadingMillis = currentMillis;
    ackData.votage = 10.6; //TODO: get real votage from FC
  }
}

void flashLed(unsigned long currentMillis) {
  if (radioMode == normal) {
    if (hasSignal) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
  } else if (radioMode == bind) {
    if (currentMillis - previousLedMillis >= ledFlashInterval) {
      previousLedMillis = currentMillis;
      ledState = !ledState;
    }
  } else {
    //unknow mode, do nothing.
  }
  digitalWrite(ledPin, ledState);
}

void setPPMValuesFromData() {
  ppm[0] = map(comData.throttle, 0, 255, 1000, 2000);
  ppm[1] = map(comData.yaw,      0, 255, 1000, 2000);
  ppm[2] = map(comData.pitch,    0, 255, 1000, 2000);
  ppm[3] = map(comData.roll,     0, 255, 1000, 2000);
  ppm[4] = map(comData.AUX1,     0, 1, 1000, 2000);
  ppm[5] = map(comData.AUX2,     0, 1, 1000, 2000);
  ppm[6] = map(comData.AUX3,     0, 1, 1000, 2000);
  ppm[7] = map(comData.AUX4,     0, 1, 1000, 2000);
}

void dumpDataToSerial() {
  Serial.print("Header: ");   Serial.print(comData.uniqueId);  Serial.print("    ");
  Serial.print("Throttle: "); Serial.print(comData.throttle);  Serial.print("    ");
  Serial.print("Yaw: ");      Serial.print(comData.yaw);       Serial.print("    ");
  Serial.print("Pitch: ");    Serial.print(comData.pitch);     Serial.print("    ");
  Serial.print("Roll: ");     Serial.print(comData.roll);      Serial.print("    ");
  Serial.print("Aux1: ");     Serial.print(comData.AUX1);      Serial.print("    ");
  Serial.print("Aux2: ");     Serial.print(comData.AUX2);      Serial.print("    ");
  Serial.print("Aux3: ");     Serial.print(comData.AUX3);      Serial.print("    ");
  Serial.print("Aux4: ");     Serial.print(comData.AUX4);      Serial.print("    ");
  Serial.print("Mode: ");     Serial.print(radioMode);         Serial.print("    ");
  Serial.print("Signal: ");   Serial.print(hasSignal);         Serial.print("    ");
  Serial.print("Channel: ");  Serial.print(channel);           Serial.print("    ");
  Serial.print("UniqueId: "); Serial.print(uniqueId);          Serial.print("    ");
  Serial.print("Votage: ");   Serial.print(ackData.votage);    Serial.print("\n");
}

void bindRadio(unsigned long currentMillis) {
#if defined(debugMode)
  Serial.print("Scanning channel "); Serial.print(channel); Serial.print("...\n");
#endif
  ackBindingData.mode = bind;
  ackBindingData.channel = channel;
  setupRadio();
  radio.setChannel(channel);
  radio.startListening();
  if (currentMillis - previousRadioBindingMillis >= radioBindingInterval) {
    previousRadioBindingMillis = currentMillis;
    while (radio.available()) {
#if defined(debugMode)
      Serial.println("Found radio: ");
#endif
      radio.read(&bindingData, sizeof(BindingData));
      radio.writeAckPayload(1, &ackBindingData, sizeof(BindingData));
    }
#if defined(debugMode)
    Serial.print("Mode: "); Serial.print(bindingData.mode); Serial.print("    ");
    Serial.print("Channel: "); Serial.print(bindingData.channel); Serial.print("    ");
    Serial.print("UniqueId: "); Serial.print(bindingData.uniqueId); Serial.print("\n");
#endif
    if (bindingData.mode == bind && bindingData.channel == channel) {
      uniqueId = bindingData.uniqueId;
      radioMode = normal;
      setChannel();
      setUniqueId();
#if defined(debugMode)
      Serial.print("Channel bound to "); Serial.print(channel); Serial.print("    ");
      Serial.print("UniqueId is "); Serial.print(uniqueId); Serial.print("\n");
#endif
      return;
    }
#if defined(debugMode)
    Serial.println("Stopping radio...");
#endif
    radio.stopListening();
    channel++;
    if (channel > channelUpperBoundery) {
      channel = channelLowerBoundery;
    }
  }
}

void getChannel() {
#if defined(debugMode)
  Serial.println("Reading channel from EEPROM...");
#endif
  channel = EEPROM.read(channelAddressEEPROM);
#if defined(debugMode)
  Serial.print("Channel is "); Serial.print(channel); Serial.print("...\n");
#endif
}

void setChannel() {
#if defined(debugMode)
  Serial.print("Writing channel to EEPROM... Channel is "); Serial.print(channel); Serial.print("...\n");
#endif
  EEPROM.write(channelAddressEEPROM, channel);
#if defined(debugMode)
  Serial.println("Write channel to EEPROM completed.");
#endif
}

void getUniqueId() {
#if defined(debugMode)
  Serial.println("Reading uniqueId from EEPROM...");
#endif
  uniqueId = EEPROM.read(uniqueIdAddressEEPROM);
#if defined(debugMode)
  Serial.print("UniqueId is "); Serial.print(uniqueId); Serial.print("...\n");
#endif
}

void setUniqueId() {
#if defined(debugMode)
  Serial.print("Writing uniqueId to EEPROM... UniqueId is "); Serial.print(uniqueId); Serial.print("...\n");
#endif
  EEPROM.write(uniqueIdAddressEEPROM, uniqueId);
#if defined(debugMode)
  Serial.println("Write uniqueId to EEPROM completed.");
#endif
}


///////////////////////////////////////////// System Timer ///////////////////////////////////////////////////////////
ISR(TIMER1_COMPA_vect) {
  static boolean state = true;

  TCNT1 = 0;

  if ( state ) {
    //end pulse
    PORTD = PORTD & ~B00000100; // turn pin 2 off. Could also use: digitalWrite(sigPin,0)
    OCR1A = PPM_PulseLen * clockMultiplier;
    state = false;
  }
  else {
    //start pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;

    PORTD = PORTD | B00000100; // turn pin 2 on. Could also use: digitalWrite(sigPin,1)
    state = true;

    if (cur_chan_numb >= channelCount) {
      cur_chan_numb = 0;
      calc_rest += PPM_PulseLen;
      OCR1A = (PPM_FrLen - calc_rest) * clockMultiplier;
      calc_rest = 0;
    }
    else {
      OCR1A = (ppm[cur_chan_numb] - PPM_PulseLen) * clockMultiplier;
      calc_rest += ppm[cur_chan_numb];
      cur_chan_numb++;
    }
  }
}
