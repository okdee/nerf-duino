#include <IRremote.h>
#include <Servo.h>

const int triggerServoPin = 8;
const int motorRelayPin = 9;
const int ceaseFireButtonPin = 10;
const int RECV_PIN = 7;

IRrecv irrecv(RECV_PIN);
decode_results results;
Servo triggerServo;

boolean motorsSpinning = false;
boolean triggerPulled = false;

const int triggerServoPulledPosition = 70;
const int triggerServoReleasedPosition = 0;

int firingDuration = 0;
const int ceaseFiringDuration = 300;
boolean ceaseFireBool = false;

void setup(){
  Serial.begin(9600);
  
  irrecv.enableIRIn();
  irrecv.blink13(true);
  
  triggerServo.attach(triggerServoPin);
  pinMode(motorRelayPin, OUTPUT);

  releaseTrigger();
  stopMotors();
  //triggerServo.write(triggerServoReleasedPosition);
  //digitalWrite(motorRelayPin, LOW);
}

void loop(){
    if (irrecv.decode(&results)){
        switch(results.value){
          case 0XFF6897: // 0 button
            ceaseFire();
            break;
          case 0xFFC23D: // >|| button 
            fullAutoFire();
            break;
          
          case 0xFF02FD: // >> button
            gradualFire(10);
            break;
          case 0xFF22DD: // << button
            gradualFire(-10);
            break;
          
          case 0xFFA857: // + button
            if (!motorsSpinning){
              spinMotors();
            }
            break;
          case 0xFFE01F: // - button
            if (!triggerPulled){
              stopMotors();
            }
            else{
              ceaseFire();
            }
            break;
          case 0xFF629D: // CH button
            if (motorsSpinning){
              pullTrigger();
              delay(500);
              releaseTrigger();
            }
            break;
        }
        delay(500);
        irrecv.resume();
    }
}

void ceaseFire(){
  Serial.println("cease fire");
  releaseTrigger();
  delay(500);
  stopMotors();
  ceaseFireBool = false;
}

void fullAutoFire(){
  Serial.println("full auto fire");
  spinMotors();
  delay(500);
  pullTrigger();
}

void spinMotors(){
  digitalWrite(motorRelayPin, HIGH);
  motorsSpinning = true;
}

void stopMotors(){
  digitalWrite(motorRelayPin, LOW);
  motorsSpinning = false;
}

void pullTrigger(){
  triggerServo.write(triggerServoPulledPosition);
  triggerPulled = true;
}

void releaseTrigger(){
  triggerServo.write(triggerServoReleasedPosition);
  triggerPulled = false;
}

void gradualFire(int rate){
  Serial.print("rate is: ");
  Serial.println(rate);
  
  int ceaseFireButtonState = LOW;
  if (firingDuration >= 0 && firingDuration <= 300)
    firingDuration += rate * 10;

    Serial.print("firig duration is: ");
    Serial.println(firingDuration);
    
    digitalWrite(motorRelayPin, HIGH);
    delay(500);
  
    while((firingDuration > 0 && firingDuration <= 300) && (!ceaseFireBool || ceaseFireButtonState == LOW)){
      Serial.println("inside gradual while loop");
      ceaseFireButtonState = digitalRead(ceaseFireButtonPin);
      if (ceaseFireButtonState == HIGH){
        Serial.println("button is HIGH");
        ceaseFireBool = true;
        ceaseFire();
        break;
      }
  
      Serial.println("button is LOW");
  
      triggerServo.write(triggerServoPulledPosition);
      delay(firingDuration);
      triggerServo.write(triggerServoReleasedPosition);
      delay(ceaseFiringDuration);
      
      if (irrecv.decode(&results)){
        Serial.print("inside decode results.value is: ");
        Serial.println(results.value);
          switch(results.value){
            case 0XFF6897: // 0 button
              ceaseFire();
              return;
              break;
            case 0xFFA857: // + button
              gradualFire(10);
              break;
            case 0xFFE01F: // - button
              gradualFire(-10);
              break;
          }
          delay(500);
          irrecv.resume();
      }
    }
}
