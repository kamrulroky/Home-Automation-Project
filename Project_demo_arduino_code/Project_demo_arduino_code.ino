#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
char auth[] = "e1f9d2372a8e4d08a856d229eb73bb5a";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Skyview Online";
char pass[] = "*****";

#define SWITCH         V1 
#define ledPin         D10               // choose the pin for the LED
#define inputPin       D0               // choose the input pin (for PIR sensor)
#define pinSpeaker     D5             //Set up a speaker on a PWM pin (digital 9, 10, or 11)
#define STATUS_DELAY   2000L         // 2 seconds in msec
#define ledpin1        D4
#define ledpin2        D3
#define btnPin1        D2
#define btnPin2        D1



int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status
SimpleTimer timer;          
bool enabled = false;
bool sendUpdate = false;


void checkPhysicalButton1();
void checkPhysicalButton2();

int led1State = LOW;
int btn1State = LOW;

int led2State = LOW;
int btn2State = LOW;

// Every time we connect to the cloud...
BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
}

// When App button is pushed - switch the state
BLYNK_WRITE(V2) {
  led1State = param.asInt();
  digitalWrite(ledpin1, led1State);
}

BLYNK_WRITE(V3) {
  led2State = param.asInt();
  digitalWrite(ledpin2, led2State);
}

void checkPhysicalButton1()
{
  if (digitalRead(btnPin1) == LOW) {
    // btnState is used to avoid sequential toggles
    if (btn1State != LOW) {

      // Toggle LED state
      led1State = !led1State;
      digitalWrite(ledpin1, led1State);

      // Update Button Widget
      Blynk.virtualWrite(V2, led1State);
    }
    btn1State = LOW;
  } else {
    btn1State = HIGH;
  }
}

void checkPhysicalButton2()
{
  if (digitalRead(btnPin2) == LOW) {
    // btnState is used to avoid sequential toggles
    if (btn2State != LOW) {

      // Toggle LED state
      led2State = !led2State;
      digitalWrite(ledpin2, led2State);

      // Update Button Widget
      Blynk.virtualWrite(V3, led2State);
    }
    btn2State = LOW;
  } else {
    btn2State = HIGH;
  }
}


// configure dashboard button V1 as a SWITCH
BLYNK_WRITE (SWITCH) {
    enabled = param.asInt();
}


// only send notifications at specified intervals to avoid flood errors
void sendNotify ( void ) {
    if ( sendUpdate ) {
        Blynk.notify("Motion detected");
         Serial.println("Motion detected");
        sendUpdate = false;
    }
}

void setup() {

  Serial.begin(115200);
  delay(10);
  Blynk.begin(auth, ssid, pass);
   
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(inputPin, INPUT);     // declare sensor as input
  pinMode(pinSpeaker, OUTPUT);
  timer.setInterval(STATUS_DELAY, sendNotify);
  pinMode(ledpin1, OUTPUT);
  pinMode(btnPin1, INPUT_PULLUP);
  
  pinMode(ledpin2, OUTPUT);
  pinMode(btnPin2, INPUT_PULLUP);
  
  digitalWrite(ledpin1, led1State);
  digitalWrite(ledpin2, led2State);

  // Setup a function to be called every 100 ms
  timer.setInterval(100L, checkPhysicalButton1);
  timer.setInterval(100L, checkPhysicalButton2);
}

void loop(){
  getPirValue();
   Blynk.run();
   timer.run();
}

void getPirValue(void){

  if ( enabled ){
  val = digitalRead(inputPin);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    digitalWrite(ledPin, HIGH);  // turn LED ON
    playTone(300, 160);
    delay(150);

    
    if (pirState == LOW) {
      // we have just turned on
      //Serial.println("Motion detected!");
      sendUpdate = true;
      // We only want to print on the output change, not state
      pirState = HIGH;
    }
  } else {
      digitalWrite(ledPin, LOW); // turn LED OFF
      playTone(0, 0);
      delay(300);    
      if (pirState == HIGH){
      // we have just turned of
      Serial.println("Motion ended!");
      // We only want to print on the output change, not state
      pirState = LOW;
    }
  }
}
  
  }
// duration in mSecs, frequency in hertz
void playTone(long duration, int freq) {
    duration *= 1000;
    int period = (1.0 / freq) * 1000000;
    long elapsed_time = 0;
    while (elapsed_time < duration) {
        digitalWrite(pinSpeaker,HIGH);
        delayMicroseconds(period / 2);
        digitalWrite(pinSpeaker, LOW);
        delayMicroseconds(period / 2);
        elapsed_time += (period);
    }
}
