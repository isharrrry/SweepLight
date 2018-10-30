#include <Servo.h>      //调用一些库文件
#include <TimedAction.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//定义舵机位置名称，并编号。
const int base =      0;     //底座舵机    
const int shoulder =   1;          
const int elbow =     2;        
const int wristflex =   3;        
const int wristrot =    4;          
const int gripper =    5;         //夹持器舵机 

const int stdDelay =   5;         //舵机运动延时（单位ms）
const int maxServos =  6;         //舵机的数量
const int centerPos =  90;        //舵机中位位置

int keyDelay = 100;              //每个按键之间的最大延时时间ms
int buttonDelay = 50;           //每个按键之间的最小延时时间ms
int thisServo = base;           //定义起始电机
unsigned long key_millis = 0;      
unsigned long button_millis = 0;

typedef struct{              //数组框架结构
  byte neutral;             //中位角度,起始位置
  byte minPos;             //最小角度
  byte maxPos;            //最大角度
  byte delaySpeed;         //延时时间
  byte curPos;            //舵机当前角度
} ServoPos;              //结构体名称

ServoPos servosPos[] = {    //对舵机限位
  { 90, 180, 10, stdDelay, 0 }, //中位90，最小角度180，最大角度10，范围0~180度。
  { 90, 180, 10, stdDelay, 0 },
  { 90, 180, 60, stdDelay, 0 },
  { 90, 170, 50, stdDelay, 0 },
  { 90, 180, 10, 10, 0 },
  { 90, 125, 55, 5, 0 }
};

byte serv = 90;
int counter = 0;
int curServo = 0;
int sMove[] = {0, 90, 0};
int sAttach[] = {0, 0};

LiquidCrystal_I2C lcd(0x27,20,4);     //0x27  D7~D0端口开关设置0x表示十六进制27转换成16进制数是00100111  1代表开，0代表关， 20列，4行，行号从零算起，第一行行号0，第二行行号1.
Servo servos[maxServos];
int destServoPos[maxServos];
int currentServoPos[maxServos];
 
void doServoFunc() {
  int x = curServo; 
  TimedAction servoMove[maxServos] = TimedAction(100, doServoFunc);
  if(destServoPos[x] == currentServoPos[x])
    servoMove[x].disable();       
  if(destServoPos[x] > currentServoPos[x])
    currentServoPos[x]++;
  else
    currentServoPos[x]--;
  
  servosPos[x].curPos = constrain(currentServoPos[x], servosPos[x].maxPos, servosPos[x].minPos);
  currentServoPos[x] = servosPos[x].curPos;
  servos[x].write(currentServoPos[x]);
  jointPos(x, currentServoPos[x]);
}

TimedAction servoMove[maxServos] = TimedAction(100, doServoFunc);   // 延时，延时时间为声明时间。
TimedAction keys = TimedAction(10, keypadFunc);

void setup() {                            //设置
  delay(200);
  Wire.begin();
  lcd.init();                             //LCD初始化             
  lcd.backlight();                 //LCD背光灯打开
  delay(500);
  lcd.on();                     // LCD开机
  setupDisplay();               //调用子程序，设置显示内容门，后面有定义。

  for(int i=0; i<maxServos; i++) {          
    servos[i].write(servosPos[i].neutral);//舵机归中位
    servosPos[i].curPos = servosPos[i].neutral;//设置舵机当前角度为初始角度
    servos[i].attach(i+4);//初始化舵机控制引脚，即第一个舵机为D4引脚控制
    destServoPos[i] = centerPos;//指定的舵机为90度
    currentServoPos[i] = centerPos;//当前的舵机为90度
    servoMove[i].disable();//舵机停止移动
  }      
}

void loop() {
  for(int x=0; x<maxServos; x++) {
    curServo = x;
    servoMove[x].check();
  }
  //timer.run();            
  keys.check();
  navSwitchFunc();
}

void servoTestFunc() {
  if(counter % 2) {
    Move(thisServo, servosPos[thisServo].minPos, servosPos[thisServo].delaySpeed);
  }
  else
    Move(thisServo, servosPos[thisServo].maxPos, servosPos[thisServo].delaySpeed);
  counter++;
}

void writeServo() {
  int servoNum = sMove[0];
  if(servoNum >=0 && servoNum <= maxServos) {
    destServoPos[servoNum] = sMove[1];
    servoMove[servoNum].enable();
    servoMove[servoNum].setInterval(sMove[2]);
  }
}

void setServoAttach() {
  int servo = 1;    // sAttach[0]
  int mode = 2;     // sAttach[1]
  if(servo >= 0 && servo <= maxServos) {
    if (mode == 1)
      servos[servo].attach(servo+4);
    else
      servos[servo].detach();
  }
}

void Move(int servoNum, int servoPosition, int delayTime) {  //舵机驱动指令
  sMove[0] = servoNum;             //所驱动舵机号
  sMove[1] = servoPosition;          //舵机的目标位置
  sMove[2] = delayTime;            //每个舵机运动的延迟时长
  writeServo();
}

void Attach(int servoNum, int servoMode) {
  sAttach[0] = servoNum;
  sAttach[1] = servoMode;
}

void navSwitchFunc() {                //键盘检测子程序
    if (millis() > button_millis + buttonDelay) {
    button_millis = millis();
    if(digitalRead(A0) == LOW) {      // 当A0引脚低电平
      thisServo--;                   //电机号自加1
      thisServo = constrain(thisServo, 0, 5); //电机控制范围
      jointPos(thisServo, servosPos[thisServo].curPos);
      delay(200);                 //延时200毫秒
    }
    if(digitalRead(A1)== LOW) {      //当A1引脚低电平
      thisServo++;                 //电机号自加1
      thisServo = constrain(thisServo, 0, 5); //电机控制范围
      jointPos(thisServo, servosPos[thisServo].curPos);
      delay(200);                 //延时200毫秒
    }
    if(digitalRead(A2) == LOW) {      //当A2引脚低电平
      byte t = thisServo;
      servosPos[t].curPos--;       //电机角度自减1
      servosPos[t].curPos = constrain(servosPos[t].curPos, servosPos[t].maxPos, servosPos[t].minPos);
      jointPos(t, servosPos[t].curPos);
    }
    if(digitalRead(A3) == LOW) {     //当A3引脚低电平
      byte t = thisServo;
      servosPos[t].curPos++;         //电机角度自减1
      servosPos[t].curPos = constrain(servosPos[t].curPos, servosPos[t].maxPos, servosPos[t].minPos);
      jointPos(t, servosPos[t].curPos);
    }
  }
}

void keypadFunc() {
  byte keypad = lcd.keypad();
  lcd.command(0);                
  if (keypad !=0) {
    if (millis() > key_millis + keyDelay) {
      keypress(keypad);
      key_millis = millis();
    }
  }
}

void keypress (byte keypad) {  //读取按键按下情况。       
  byte t;
  lcd.setCursor(15, 1);
  switch(keypad) {
    case 1: 
      lcd.print("1");
      t = base;
      servosPos[t].curPos--;
      break;
    case 2:
      lcd.print("2");
      t = shoulder;
      servosPos[t].curPos--;
      break;
    case 3:
      lcd.print("3");
      t = elbow;
      servosPos[t].curPos--;
      break;
    case 5:
      lcd.print("4");
      t = base;
      servosPos[t].curPos++;
      break;
    case 6:
      lcd.print("5");
      t = shoulder;
      servosPos[t].curPos++;
      break;
    case 7:
      lcd.print("6");
      t = elbow;
      servosPos[t].curPos++;
      break;
    case 9:
      lcd.print("7");
      t = wristflex;
      servosPos[t].curPos--;
      break;
    case 10:
      lcd.print("8");
      t = wristrot;
      servosPos[t].curPos--;
      break;
    case 11:
      lcd.print("9");
      t = gripper;
      servosPos[t].curPos--;
      break;
    case 13:
      lcd.print("*");
      t = wristflex;
      servosPos[t].curPos++;
      break;
    case 14:
      lcd.print("0");
      t = wristrot;
      servosPos[t].curPos++;
      break;
    case 15:
      lcd.print("#");
      t = gripper;
      servosPos[t].curPos++;
      break;   
  }
  servosPos[t].curPos = constrain(servosPos[t].curPos, servosPos[t].maxPos, servosPos[t].minPos);
  jointPos(t, servosPos[t].curPos);
}

void jointPos(byte t, byte pos) {    //定义两个byte类型的变量，t,pose.
  lcd.setCursor(6, 1);//电机所在位置名称
  switch(t) {
    case base:               // 1, 4
      lcd.print("bse");    //LCD显示bse
      break;
    case shoulder:           // 2, 5     
      lcd.print("shl");   //LCD显示shl
      break;
    case elbow:              // 3, 6
      lcd.print("elb");    //LCD显示elb
      break;
    case wristflex:          // 7, * 
      lcd.print("wfx");   //LCD显示wfx
      break;
    case wristrot:           // 8, 0    
      lcd.print("wrt");   //LCD显示wrt
      break;
    case gripper:            // 9, #
      lcd.print("grp");    //LCD显示grp
      break;
  }
  
  lcd.setCursor(2, 3);// 设置显示位置第4行，3列就是说前面空2格。
  lcd.print(" you are welcome  ");
  lcd.setCursor(6, 2);//pose后面显示的角度位置。
  lcd.print(pos, DEC); //当按键按下后显示角度值。
  servos[t].write(pos);
}

void setupDisplay() {    //子程序定义，LCD显示内容，开机立即显示。
  lcd.clear();          //LCD清屏
  lcd.blink_off();       //LCD光标闪烁关
  lcd.home();         //
  lcd.setCursor(1, 1);  //设置显示位置第2行，2列就是说前面空1格。
  lcd.print("key: ");   //显示内容 “key”
  lcd.setCursor(4, 0);   //设置显示位置第一行，5列就是说前面空4格。
  lcd.print("alsrobotbase      ");//显示内容”alsrobotbase”
  lcd.setCursor(1, 2);         // 设置显示位置
  lcd.print("Pos: ");           //显示内容”Pos”
}
