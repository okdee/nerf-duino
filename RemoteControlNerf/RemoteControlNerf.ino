#include <IRremote.h>
#include <Servo.h>

Servo triggerServo;

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;

const int triggerServoPin = 8;
const int motorRelayPin = 9;

boolean fireTrigger = false;

void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
  
  triggerServo.attach(triggerServoPin);
  pinMode(motorRelayPin, OUTPUT);

  digitalWrite(motorRelayPin, LOW);
  triggerServo.write(0);
}

void loop(){
    if (irrecv.decode(&results)){
        switch(results.value){
          case 0xFFC23D: //Keypad button >||
            fireTrigger = !fireTrigger;
            if (!fireTrigger){
              //Serial.println("stop");
              triggerServo.write(0);
              delay(500);
              digitalWrite(motorRelayPin, LOW);
            }
            else{
              //Serial.println("fire");
              digitalWrite(motorRelayPin, HIGH);
              delay(500);
              triggerServo.write(70);
            }
            break;
          }
        delay(500);
        irrecv.resume();
    }
}
