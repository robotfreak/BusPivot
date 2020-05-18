/* Bus Pivot:
 *  The bus is on a pivot and will act like a seesaw. It will be flooded with water,
 *  until it begins to tilt,
 *  A acc/gyro chip will measure the tilt angle
 *  When the bus hits the ground, the rear door will be open and spill the water out
 *  Once the water has spilled out the bus will tip back and the door will be closed
 *  
 *  Arduino libraries need: MPU6050_tockn (install via Library manager)
 *  created by robotfreak p.recktenwald@gmail.com
 */

//#include <MPU6050_tockn.h>
#include <Wire.h>
#include <Servo.h>

#define _DEBUG   // Uncomment to enable debug output (e.g. show angle in serial plotter)

const int servoPin = 9;       //pin to connect the door servo
const int switchPin = 3;      //pin for the switch
//MPU6050 mpu6050(Wire);
Servo doorServo;

const int DOOR_OPEN = 130;     //servo postion for door open
const int DOOR_CLOSE = 70;    //servo postion for door close
const int DOOR_MIDDLE = 100;   // middle position for door

//const int TILT_ANGLE_MAX = 15;    //max tilt angle to open the door
//const int TILT_ANGLE_MIN = 1;     //min tilt angle to open the door

const int OPEN_DELAY = 1500;       //delay time (in ms) to leave the door open
const int OPEN_TIME  = 300;        //time to open the door
const int CLOSE_TIME = 3000;       //time to close the door

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
int servoPos = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Bus Pivot v0.2");
  Wire.begin();
//  mpu6050.begin();
//  mpu6050.calcGyroOffsets(true);
  doorServo.attach(servoPin);
  doorServo.write(DOOR_CLOSE);
  servoPos = DOOR_CLOSE;
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, LOW);
  state = S_INIT;
  timer = 0;
  Serial.print("servoOpenSteps: ");
  Serial.println(servoOpenSteps, DEC);
  Serial.print("servoCloseSteps: ");
  Serial.println(servoCloseSteps, DEC);
  Serial.println("\n>INIT");
}

void loop() {
  int swi;
  //int x,y,z;
  //mpu6050.update();
  //x = mpu6050.getAngleX();
  //y = mpu6050.getAngleY();
  //z = mpu6050.getAngleZ();
  swi = digitalRead(switchPin);
#ifdef _DEBUG  
  //Serial.print(x);
  //Serial.print(",");
  //Serial.print(y);
  //Serial.print(",");
  Serial.println(servoPos);
#endif  
  //Serial.print("angleX : ");
  //Serial.print(mpu6050.getAngleX());
  //Serial.print(,);
  //Serial.print("\tangleY : ");
  //Serial.print(mpu6050.getAngleY());
  //Serial.print("\tangleZ : ");
  //Serial.println(mpu6050.getAngleZ());

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
    break;
    case S_OPEN:
      if (timer>0) {
        timer-=10;
        if ((timer % 100) == 0) Serial.print(".");
        delay(10);
        //servoPos += servoOpenSteps;
        doorServo.write(servoPos++);
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
        servoPos = DOOR_OPEN;
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
          doorServo.write(servoPos--);
        }
        else {
          Serial.println(">INIT");
          state = S_INIT;
        }
      }
    break;
    default:
      state = S_INIT;
    break;
     
  }
}
