/*
   Harrrry
   20181029
   机械臂寻光

   循环6轴进行寻光
   每轴在左右10度范围内扫描建立采样光度表，并跳转到最亮位置

*/

#include <Servo.h>

Servo myservo[6];
#define MIN 0
#define MAX 1
#define SER_POS_NUM 50
int lightVal[SER_POS_NUM] = {0};
int serPosNow[6] = {0};
int serNo = 0;

int serPin[] =
{
  9, 8, 7, 6, 5, 4
};
//最小角度180，最大角度10，范围0~180度。

int serPos[][2] =
{
  {55, 125},
  {10, 180},
  {100, 180},
  {60, 180},
  {120, 180},
  {10, 180},
};

void setup() {
  Serial.begin(9600);
  delay(1000);
  //init
  for (int j = 0; j < 6; j++)
  {
    serNo = j;
    myservo[serNo].attach(serPin[serNo]);
  }
  for (int j = 5; j >= 0; j--)
  {
    serNo = j;
    serPosNow[serNo] = 120;
    movePos( myservo[serNo].read(),  serPosNow[serNo], 10);
  }

}
//int oldPos;
void loop() {
  for (int i = 0; i < 6; i++)
  {
    //循环6轴
    serNo = i;

    //规划角度
    if (serPosNow[serNo] - SER_POS_NUM / 2 < serPos[serNo][MIN])
      serPosNow[serNo] = serPos[serNo][MIN] + SER_POS_NUM / 2;
    if (serPosNow[serNo] + SER_POS_NUM / 2 > serPos[serNo][MAX])
      serPosNow[serNo] = serPos[serNo][MAX] - SER_POS_NUM / 2;

    //扫描
    int startPos = serPosNow[serNo] - SER_POS_NUM / 2;
    serPosNow[serNo] = startPos;
    //    Serial.print("\t oldPos = ");
    //    Serial.print(oldPos);
    Serial.print("\t startPos = ");
    Serial.print(startPos);
    Serial.println();
    //到位
    movePos( myservo[serNo].read(), startPos, 40);
    //最左开扫， 每轴在左右10度范围内扫描建立采样光度表
    for (int j = 0; j < SER_POS_NUM; j++)
    {
      serPosNow[serNo]++;
      myservo[serNo].write(serPosNow[serNo]);
      delay(80);
      lightVal[j] = analogRead(A5);
    }

    //最亮位置
    int lightMax = 0;
    for (int j = 0; j < SER_POS_NUM; j++)
    {
      if (lightVal[j] > lightVal[lightMax])
        lightMax = j;
    }

    //跳转到位置
    serPosNow[serNo] = startPos + lightMax;
    //到位
    movePos( myservo[serNo].read(), serPosNow[serNo], 40);
    delay(2000);
  }
}
unsigned long  Time;
//void sysDelay(unsigned long ms) {
//  TimeCount(0, &Time);
//  //保持舵机锁住位置
//  while (TimeCount(1, &Time) < ms)
//  {
//    for (int j = 0; j < 6; j++)
//    {
//      serNo = j;
//      myservo[serNo].attach(serPin[serNo]);  // attaches the servo on pin 9 to the servo object
//      delay(1);
//      myservo[serNo].write(serPosNow[serNo]);
//      delay(1);
//    }
//  }
//}
//简单时间控制 17D
//0开始，1取结果,2重置并取结果
unsigned long TimeCount(char _do, unsigned long * Time)
{
  if (_do == 0)
    *Time = millis();
  else if (_do == 1)
    return millis() - *Time;
  else
  {
    unsigned long t = millis() - *Time;
    *Time = millis();
    return t;
  }
}

void movePos(int oldPos, int objPos, int stepMs)
{
  static int staticMoveVal;
  if (oldPos > objPos)
  {
    for (staticMoveVal = oldPos ; staticMoveVal > objPos; staticMoveVal--)
    {
      myservo[serNo].write(staticMoveVal);
      delay(stepMs);
    }
  }
  else
  {
    for (staticMoveVal = oldPos; staticMoveVal < objPos; staticMoveVal++)
    {
      myservo[serNo].write(staticMoveVal);
      //      Serial.print("\t: ");
      //      Serial.print(staticMoveVal);
      //      Serial.println();
      delay(stepMs);
    }
  }
}
