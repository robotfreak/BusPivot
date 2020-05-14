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

#include <MPU6050_tockn.h>
#include <Wire.h>
#include <Servo.h>

//#define _DEBUG   // Uncomment to enable debug output (e.g. show angle in serial plotter) 
const char version[4] = "v1.0";
const int servoPin = 9;       //pin to connect the door servo

MPU6050 mpu6050(Wire);
Servo doorServo;

const int DOOR_OPEN = 20;     //servo postion for door open
const int DOOR_CLOSE = 90;    //servo postion for door close

const int TILT_ANGLE_MAX = 15;    //max tilt angle to open the door
const int TILT_ANGLE_MIN = 1;     //min tilt angle to open the door
const int OPEN_DELAY = 4000;       //delay time (in ms) to leave the door open 

enum eState {
  S_INIT,
  S_OPEN,
  S_CLOSE
};

enum eState state = S_INIT;
int timer = 0;

void setup() {
  Serial.begin(9600);
  Serial.print("Bus Pivot ");
  Serial.println(version);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  doorServo.attach(servoPin);
  doorServo.write(DOOR_CLOSE);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  state = S_INIT;
  timer = 0;
  Serial.println("\n>INIT");
}

void loop() {
  int x,y,z;
  mpu6050.update();
  x = mpu6050.getAngleX();
  y = mpu6050.getAngleY();
  z = mpu6050.getAngleZ();
#ifdef _DEBUG  
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(",");
  Serial.println(z);
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
      if (x >= TILT_ANGLE_MAX) { // when max seesaw angle has been reached
        digitalWrite(LED_BUILTIN, HIGH);
        doorServo.write(DOOR_OPEN);  // open the door
        timer = OPEN_DELAY;
        Serial.println(">OPEN");
        state = S_OPEN;
      }
    break;
    case S_OPEN:
      if (timer) {
        timer--;
        if ((timer % 100) == 0) Serial.print(".");
        delay(1);
      }
      else {
        doorServo.write(DOOR_CLOSE); // close the door
        Serial.println("\n>CLOSE");
        state = S_CLOSE;
      }
    break;
    case S_CLOSE:
      if (x <= TILT_ANGLE_MIN) {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println(">INIT");
        state = S_INIT;
      }
    break;
    default:
      state = S_INIT;
    break;
      
  }
}
