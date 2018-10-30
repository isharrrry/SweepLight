/* Sweep
  by BARRAGAN <http://barraganstudio.com>
  This example code is in the public domain.

  modified 8 Nov 2013
  by Scott Fitzgerald
  http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 2;    // variable to store the servo position
int serNo = 0;
int serPin[] =
{
  9, 8, 7, 6, 5, 4
};
int serPos[][2] =
//最小角度180，最大角度10，范围0~180度。
{
  {55, 125},
  {10, 180},
  {100, 180},
  {60, 180},
  {120, 180},
  {10, 180},
};

void setup() {
  myservo.attach(serPin[serNo]);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  for (pos = serPos[serNo][0]; pos <= serPos[serNo][1]; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 15ms for the servo to reach the position
  }
  for (pos = serPos[serNo][1]; pos >= serPos[serNo][0]; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 15ms for the servo to reach the position
  }
}

