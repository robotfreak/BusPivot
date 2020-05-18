/* Bus Pivot:
 *  The bus is on a pivot and will act like a seesaw. It will be flooded with water, 
 *  until it begins to tilt, 
 *  A button will detect the tilt
 *  When the bus hits the ground, the door will be open and spill the water out 
 *  Once the water has spilled out the bus will tip back and the door will be closed
 *  If the bus will get stuck, a second servo will push it back on track 
 *  
 *  created by robotfreak p.recktenwald@gmail.com
 */

#include <Wire.h>
#include <Servo.h>

//#define _DEBUG   // Uncomment to enable debug output (e.g. show angle in serial plotter)
const char version[5] = "v2.1";

const int doorServoPin = 9;       //pin to connect the door servo
const int pushServoPin = 7;   //pin to connect the push servo
const int switchPin = 3;      //pin for the switch
Servo doorServo;
Servo pushServo;

// door servo positions
const int DOOR_OPEN = 130;     //servo postion for door open
const int DOOR_CLOSE = 70;    //servo postion for door close
const int DOOR_MIDDLE = 100;   // middle position for door

// push servo positions
const int PUSH_HOMEPOS = 20;
const int PUSH_ACTIVE = 150;

const int OPEN_DELAY = 1500;       //delay time (in ms) to leave the door open
const int OPEN_TIME  = 300;        //time to open the door (*10ms)
const int CLOSE_TIME = 3000;       //time to close the door (*10ms)
const int PUSH_TIMEOUT = 4500;     //timeout for push back servo (*10ms)

int servoOpenSteps = OPEN_TIME/10;   //*10ms
int servoCloseSteps = CLOSE_TIME/50;  // *100ms

enum eState {
  S_INIT,
  S_OPEN,
  S_CLOSE,
  S_CLOSING
};

enum eState state = S_INIT;
int timer = 0;
int timeout = 0;
int doorServoPos = 0;
int pushServoPos = 0;

void setup() {
  Serial.begin(9600);
  Serial.print("Bus Pivot ");
  Serial.println(version);
  Wire.begin();

  doorServo.attach(doorServoPin);
  doorServo.write(DOOR_CLOSE);
  doorServoPos = DOOR_CLOSE;
  pushServo.attach(pushServoPin);
  pushServo.write(PUSH_HOMEPOS);
  pushServoPos = PUSH_HOMEPOS;

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, LOW);
  state = S_INIT;
  timer = 0;
  timeout = PUSH_TIMEOUT;

  Serial.print("servoOpenSteps: ");
  Serial.println(servoOpenSteps, DEC);
  Serial.print("servoCloseSteps: ");
  Serial.println(servoCloseSteps, DEC);
  Serial.println("\n>INIT");
}

void loop() {
  int swi;
  swi = digitalRead(switchPin);
#ifdef _DEBUG  
  //Serial.print(x);
  //Serial.print(",");
  //Serial.print(y);
  //Serial.print(",");
  Serial.println(doorServoPos);
#endif  

  switch(state) {
    case S_INIT:
      if (swi == LOW) { // when max seesaw angle has been reached
        digitalWrite(LED_BUILTIN, HIGH);
        // doorServo.write(DOOR_MIDDLE);  // open the door
        //servoPos = DOOR_MIDDLE;
        timer = OPEN_TIME;
        Serial.println(">OPEN");
        state = S_OPEN;
        delay(15000);
      }
      else if (timeout>0) {
        timeout--;
        if ((timeout % 100) == 0) Serial.print(".");
        delay(10);
      }
      else {
        Serial.println(">TIMEOUT");
        pushServo.write(PUSH_ACTIVE);
        delay(2000);
        pushServo.write(PUSH_HOMEPOS);
        timeout = PUSH_TIMEOUT;
      }
    break;
    case S_OPEN:
      if (timer>0) {
        timer-=10;
        if ((timer % 100) == 0) Serial.print(".");
        delay(10);
        //servoPos += servoOpenSteps;
        doorServo.write(doorServoPos++);
      }
      else {
        doorServo.write(DOOR_OPEN); // open the door
        Serial.println("\n>CLOSE");
        state = S_CLOSE;
      }
    break;
   
    case S_CLOSE:
      if (swi == LOW) {
        digitalWrite(LED_BUILTIN, LOW);
        state = S_CLOSING;
        timer = CLOSE_TIME;
        doorServoPos = DOOR_OPEN;
        state = S_CLOSING;
      }
    break;
    case S_CLOSING:
      if (swi == HIGH) {
        if (timer>0) {
          timer -= 50;
          if ((timer%100) == 0) Serial.print('.');
          delay(50);
          //servoPos -= servoCloseSteps;
          doorServo.write(doorServoPos--);
        }
        else {
          Serial.println(">INIT");
          state = S_INIT;
          timeout = PUSH_TIMEOUT;
        }
      }
    break;
    default:
      state = S_INIT;
      timeout = PUSH_TIMEOUT;
    break;
     
  }
}
