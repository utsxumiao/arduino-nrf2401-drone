#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Bounce2.h>

//==== Pin Definition ===================================================//
// Pin 9, 10, 11, 12, 13 are used to connect NRF module
// Becareful: when use A6, A7 pin
//      A6, A7 are analog input only, also have no internal pullup.
//      You will need to add external pullup/pulldown resistor for them.
#define throttlePin A0
#define yawPin      A1
#define pitchPin    A2
#define rollPin     A3
#define aux1Pin     2   //Arm states
#define aux2Pin     3   //Flight modes
#define aux3Pin     A4  //Bomb drop
#define aux4Pin     A5  //Beacon
//#define aux5Pin     A6
//#define aux6Pin     A7
#define receiverPin 4
#define greenLedPin 5   // Arm states
#define redLedPin   6   // Receiver numbers
#define blueLedPin  7   // Flight modes
#define buzzerPin   8
//=======================================================================//


//Set receivers ID this transmitter can communicate to
int receiverNames[] = {1, 2, 3, 4};
int radioChannelLowerBoundery = 110;
int radioChannelUpperBoundery = 125;

int aux1Value;
int aux1LowerBoundery = 0;
int aux1UpperBoundery = 1;
int aux2Value;
int aux2LowerBoundery = 0;
int aux2UpperBoundery = 2;
int aux3Value;
int aux3LowerBoundery = 0;
int aux3UpperBoundery = 1;
int aux4Value;
int aux4LowerBoundery = 0;
int aux4UpperBoundery = 1;
int receiverValue;
int receiverLowerBoundery = 0;
int receiverUpperBoundery = sizeof(receiverNames)/sizeof(int) - 1;

boolean aux1Pressed = false;
boolean aux2Pressed = false;
boolean aux3Pressed = false;
boolean aux4Pressed = false;
boolean receiverPressed = false;

Bounce bounce1 = Bounce();
Bounce bounce2 = Bounce();
Bounce bounce3 = Bounce();
Bounce bounce4 = Bounce();
Bounce bounceReceiver = Bounce();

int buzzerTimesToBeep = 0;
int buzzerState = LOW;
const long buzzerBeepInterval = 50;
unsigned long previousBuzzerMillis = 0;

int greenLedState = LOW;
const long greenLedFlashInterval = 200;
unsigned long previousGreenLedMillis = 0;

int redLedTimesToFlash = 0;
int redLedState = LOW;
const long redLedFlashInterval = 100;
const long redLedFlashRestInterval = 2000;
unsigned long previousRedLedMillis = 0;
unsigned long previousRedLedRestMillis = 0;

int blueLedState = LOW;
const long blueLedSlowFlashInterval = 400;
const long blueLedQuickFlashInterval = 200;
unsigned long previousBlueLedMillis = 0;


/*Create a unique pipe out. The receiver has to wear the same unique code*/
const uint64_t pipeOut = 0xE8E8F0F0E1LL; //IMPORTANT: The same as in the receiver
const int channel = 120;
RF24 radio(9, 10); // CE, CSN  pin


// The sizeof this struct should not exceed 32 bytes
// This gives us up to 32 8 bits channals
struct MyData {
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
struct AckData {
  float votage;
};

AckData ackData;
MyData data;

void resetData()
{
  //This are the start values of each channal
  // Throttle is 0 in order to stop the motors
  //127 is the middle value of the 10ADC.
  data.throttle = 0;
  data.yaw = 127;
  data.pitch = 127;
  data.roll = 127;
  data.AUX1 = 0;
  data.AUX2 = 0;
  data.AUX3 = 0;
  data.AUX4 = 0;
}

void setup()
{
  Serial.begin(115200);
  //Serial.println("System starting...");
  //Start everything up
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(greenLedPin, HIGH);
  digitalWrite(redLedPin, HIGH);
  digitalWrite(blueLedPin, HIGH);
  digitalWrite(buzzerPin, HIGH);
  
  aux1Value = aux1LowerBoundery;
  aux2Value = aux2LowerBoundery;
  aux3Value = aux3LowerBoundery;
  aux4Value = aux4LowerBoundery;
  receiverValue = receiverLowerBoundery;
  bounce1.attach(aux1Pin);
  bounce2.attach(aux2Pin);
  bounce3.attach(aux3Pin);
  bounce4.attach(aux4Pin);
  bounceReceiver.attach(receiverPin);
  bounce1.interval(5);
  bounce2.interval(5);
  bounce3.interval(5);
  bounce4.interval(5);
  bounceReceiver.interval(5);

  //Serial.println("Initialsing NRF24L01 module...");
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  //radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.setChannel(channel);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
  //Serial.println("NRF24L01 module is ready.");

  delay(1000);
  resetData();
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);
  digitalWrite(blueLedPin, LOW);
  digitalWrite(buzzerPin, LOW);
  //Serial.println("System is ready.");
}

/**************************************************/

// Returns a corrected value for a joystick position that takes into account
// the values of the outer extents and the middle of the joystick range.
int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper);
  if ( val < middle )
    val = map(val, lower, middle, 0, 128);
  else
    val = map(val, middle, upper, 128, 255);
  return ( reverse ? 255 - val : val );
}

int revertButtonValue(int val) {
  if (val == 0)
    return 1;
  else
    return 0;
}

// Set buttons value
void setButtonsValue(){
  bounce1.update();
  bounce2.update();
  bounce3.update();
  bounce4.update();
  bounceReceiver.update();

  if(bounce1.read() == LOW){
    if(!aux1Pressed){
      aux1Value++;
      if(aux1Value > aux1UpperBoundery){
        aux1Value = aux1LowerBoundery;
      }
      aux1Pressed = true;
      buzzerTimesToBeep = aux1Value + 1;
    }
  }else{
    aux1Pressed = false;
  }

  if(bounce2.read() == LOW){
    if(!aux2Pressed){
      aux2Value++;
      if(aux2Value > aux2UpperBoundery){
        aux2Value = aux2LowerBoundery;
      }
      aux2Pressed = true;
      buzzerTimesToBeep = aux2Value + 1;
    }
  }else{
    aux2Pressed = false;
  }

  if(bounce3.read() == LOW){
    if(!aux3Pressed){
      aux3Value++;
      if(aux3Value > aux3UpperBoundery){
        aux3Value = aux3LowerBoundery;
      }
      aux3Pressed = true;
      buzzerTimesToBeep = aux3Value + 1;
    }
  }else{
    aux3Pressed = false;
  }

  if(bounce4.read() == LOW){
    if(!aux4Pressed){
      aux4Value++;
      if(aux4Value > aux4UpperBoundery){
        aux4Value = aux4LowerBoundery;
      }
      aux4Pressed = true;
      buzzerTimesToBeep = aux4Value + 1;
    }
  }else{
    aux4Pressed = false;
  }

  if(bounceReceiver.read() == LOW){
    if(aux1Value > 0){
      // Any kinds of Armed state, prevent from disconnection
      buzzerTimesToBeep = 10;
    }else{
      if(!receiverPressed){
        receiverValue++;
        if(receiverValue > receiverUpperBoundery){
          receiverValue = receiverLowerBoundery;
        }
        receiverPressed = true;
        buzzerTimesToBeep = receiverValue + 1;
      }
    }
  }else{
    receiverPressed = false;
  }
}

void flashLeds(unsigned long currentLedMillis){
  flashGreenLed(currentLedMillis);
  flashRedLed(currentLedMillis);
  flashBlueLed(currentLedMillis);
}

void flashGreenLed(unsigned long currentLedMillis){
  if(aux1Value == 0){
    //Not armed
    greenLedState = HIGH;
  }else if(aux1Value == 1){
    //Armed, flash it
    if(currentLedMillis - previousGreenLedMillis >= greenLedFlashInterval){
      previousGreenLedMillis = currentLedMillis;
      greenLedState = !greenLedState;
//      if(greenLedState == LOW){
//        greenLedState = HIGH;
//      }else{
//        greenLedState = LOW;
//      }
    }
  }else{
    //Out of range
  }
  digitalWrite(greenLedPin, greenLedState);
}

void flashRedLed(unsigned long currentLedMillis){
  if(currentLedMillis - previousRedLedRestMillis >= redLedFlashRestInterval){
    previousRedLedRestMillis = currentLedMillis;
    redLedTimesToFlash = receiverValue + 1;
  }
  if(redLedTimesToFlash > 0){
    if(currentLedMillis - previousRedLedMillis >= redLedFlashInterval){
      previousRedLedMillis = currentLedMillis;
      redLedState = !redLedState;
      if(redLedState == LOW){
        redLedTimesToFlash--;
      }
    }
  }else{
    redLedState = LOW;
  }
  digitalWrite(redLedPin, redLedState);
}

void flashBlueLed(unsigned long currentLedMillis){
  if(aux2Value == 0){
    //Balanced
    blueLedState = HIGH;
  }else if(aux2Value == 1){
    //Horizon, flash slowly
    if(currentLedMillis - previousBlueLedMillis >= blueLedSlowFlashInterval){
      previousBlueLedMillis = currentLedMillis;
      blueLedState = !blueLedState;
    }
  }else if(aux2Value == 2){
    //Acro, flash quickly
    if(currentLedMillis - previousBlueLedMillis >= blueLedQuickFlashInterval){
      previousBlueLedMillis = currentLedMillis;
      blueLedState = !blueLedState;
    }
  }else{
    //Out of range
  }
  digitalWrite(blueLedPin, blueLedState);
}

void beepBuzzer(unsigned long currentBuzzerMillis){
  if(buzzerTimesToBeep > 0 || buzzerState == HIGH){
    if(currentBuzzerMillis - previousBuzzerMillis >= buzzerBeepInterval){
      previousBuzzerMillis = currentBuzzerMillis;
      if(buzzerState == LOW){
        buzzerState = HIGH;
        buzzerTimesToBeep--;
      }else{
        buzzerState = LOW;
      }
      digitalWrite(buzzerPin, buzzerState);
//      Serial.print("setting buzzer pin to ");
//      Serial.print(buzzerState);
//      Serial.print("    TimesToBeep: ");
//      Serial.print(buzzerTimesToBeep);
//      Serial.print("\n");
    }
  }
}

float votage = 0;
int sendCount = 0;
int ackCount = 0;
const long countInterval = 1000;
unsigned long previousCountMillis = 0;

//////////////////////Test
enum RadioMode {
  normal,
  bind
};
RadioMode radioMode = bind;

struct BindingData {
  byte mode;
  byte channel;
  byte uniqueId;
};
BindingData bindingData;
BindingData ackBindingData;


void loop()
{
  setButtonsValue();
  unsigned long currentMillis = millis();
  beepBuzzer(currentMillis);
  flashLeds(currentMillis);
  
  // The calibration numbers used here should be measured
  // for your joysticks till they send the correct values.
  data.uniqueId   = 111;
  data.throttle = mapJoystickValues( analogRead(throttlePin), 1, 510, 1020, false );
  data.yaw      = mapJoystickValues( analogRead(yawPin),  1, 518, 1020, true );
  data.pitch    = mapJoystickValues( analogRead(pitchPin), 1, 518, 1020, false );
  data.roll     = mapJoystickValues( analogRead(rollPin), 1, 506, 1020, true );
  data.AUX1     = aux1Value;
  data.AUX2     = aux2Value;
  data.AUX3     = aux3Value;
  data.AUX4     = aux4Value;

//  Serial.print("Header: ");   Serial.print(data.header);    Serial.print("    ");
//  Serial.print("Throttle: "); Serial.print(data.throttle);  Serial.print("    ");
//  Serial.print("Yaw: ");      Serial.print(data.yaw);       Serial.print("    ");
//  Serial.print("Pitch: ");    Serial.print(data.pitch);     Serial.print("    ");
//  Serial.print("Roll: ");     Serial.print(data.roll);      Serial.print("    ");
//  Serial.print("Aux1: ");     Serial.print(data.AUX1);      Serial.print("    ");
//  Serial.print("Aux2: ");     Serial.print(data.AUX2);      Serial.print("    ");
//  Serial.print("Aux3: ");     Serial.print(data.AUX3);      Serial.print("    ");
//  Serial.print("Aux4: ");     Serial.print(data.AUX4);      Serial.print("\n");
//
  if(radio.write(&data, sizeof(MyData))){
    sendCount++;
    if(radio.isAckPayloadAvailable()){
      radio.read(&ackData, sizeof(AckData));
      ackCount++;
      Serial.println(ackData.votage);
    }
  }

//    bindingData.mode = bind;
//    bindingData.channel = 120;
//    bindingData.uniqueId = 111;
//    if(radio.write(&bindingData, sizeof(BindingData))){
//      Serial.println("Binding data sent.");
//      if(radio.isAckPayloadAvailable()){
//        radio.read(&ackBindingData, sizeof(BindingData));
//        Serial.print("Mode: ");     Serial.print(ackBindingData.mode);      Serial.print("    ");
//        Serial.print("Channel: ");  Serial.print(ackBindingData.channel);   Serial.print("    ");
//        Serial.print("UniqueId: "); Serial.print(ackBindingData.uniqueId);  Serial.print("\n");
//        bindingData.mode = normal;
//      }
//    }
//      Serial.print("Mode: ");     Serial.print(bindingData.mode);      Serial.print("    ");
//  Serial.print("Channel: ");     Serial.print(bindingData.channel);      Serial.print("    ");
//  Serial.print("UniqueId: ");     Serial.print(bindingData.uniqueId);      Serial.print("\n");
  
  if(currentMillis - previousCountMillis >= countInterval){
    previousCountMillis = currentMillis;
    if(votage <= 10.5 && votage > 0){
      buzzerTimesToBeep = 5;
    }
//    if(ackCount/sendCount < 0.8){
//      buzzerTimesToBeep = 5;
//    }
//    if(ackData.throttle > 200){
//      buzzerTimesToBeep = 5;
//    }else{
//      buzzerTimesToBeep = 0;
//    }
    //Serial.print(sendCount); Serial.print("    "); Serial.print(ackCount); Serial.print("    "); Serial.print(ackCount/sendCount); Serial.print("\n");
    ackCount = 0;
    sendCount = 0;
  }
}
