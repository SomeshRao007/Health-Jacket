

#define LED 2

void setup() {
  // Set pin mode
  pinMode(LED,OUTPUT);
  //Serial.begin(115200);
}

void loop() {
  
  digitalWrite(LED,HIGH);
  delay(500);
  digitalWrite(LED,LOW);
  delay(500);
}
