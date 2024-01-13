#define BLYNK_PRINT Serial

#define BLYNK_USE_DIRECT_CONNECT

#define Rled 34
//#define Bled 35

#include <BlynkSimpleEsp32_BT.h>

char auth[] = "YourAuthToken";

const int buttonPin = 4;  // the number of the pushbutton pin
//const int ledPin =  5;    // the number of the LED pin

// variable for storing the pushbutton status 
int buttonState = 0;

// constants won't change. Used here to set a pin number:
const int BledPin = 35 ;// the number of the LED pin

// Variables will change:
int ledState = LOW;             // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;        // will store last time LED was updated

// constants won't change:
const long interval = 500;           // interval at which to blink (milliseconds)

void setup() {
  
  pinMode(buttonPin, INPUT);
  //pinMode(ledPin, OUTPUT);
  pinMode(Rled, OUTPUT);
  //pinMode(Bled, OUTPUT);

  // set the digital pin as output:
  pinMode(BledPin, OUTPUT);

  // Debug console
  Serial.begin(9600);

  Serial.println("Waiting for connections...");

  Blynk.setDeviceName("Blynk");

  Blynk.begin(auth);
  
}

void loop() {

  // read the state of the pushbutton value
  buttonState = digitalRead(buttonPin);
  
  
  if (buttonState == HIGH) {
    Blynk.run();
     digitalWrite(Rled, HIGH);
     
  } else {
    // turn LED off
    digitalWrite(Rled, LOW);
  }

  // here is where you'd put code that needs to be running all the time.

  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(BledPin, ledState);
  }
  
}
