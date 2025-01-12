#include <Servo.h> 
//Subscript indicates Right, Left, Up, Down for the turning servos, and East, West, North, and South for the engage servos all relative to the camera
Servo servoR, servoL, servoU, servoD, servoE, servoW, servoN, servoS;

//Time constants
#define timeDelay 500 //in miliseconds
#define halfTurn 500 //in MillisecondsW
#define quarterTurn 350 //in Milliseconds
#define engageTime 375 //in Milliseconds

//Engage constants, servo Engage angle in degrees is about 145 degrees
#define Eengage 500
#define Edisengage 1350 //screwed in at 1600
#define Wengage 1480
#define Wdisengage 750 //screwed in at 500
#define Nengage 500
#define Ndisengage 1300 //screwed in at 1635
#define Sengage 1650
#define Sdisengage 800 //screwed in at 500

//0-180 for all servos is clockwise | 180-0 is counter clockwise
//0 For East+North servos is Engage (capital char) | engageAngle is Disengage (lowercase char)
//engageAngle For West+South servos is Engage (capital char) | 0 is Disengage (lowercase char)

void execute(char str[], int len){
  int start = 0, angle;
  for(int i = 0; i <= len; i++){
    if(str[i] == ' ' || str[i] == '.' || i == len){
      angle = (str[start+1] - '0')*700 + 500;
      switch (str[start]) {
        case 'R':
          servoR.writeMicroseconds(angle);
          break;
        case 'L':
          servoL.writeMicroseconds(angle);
          break;
        case 'U':
          servoU.writeMicroseconds(angle);
          break;
        case 'D':
          servoD.writeMicroseconds(angle);
          break;
        case 'E':
          servoE.writeMicroseconds(Eengage);
          break;
        case 'e':
          servoE.writeMicroseconds(Edisengage);
          break;
        case 'W':
          servoW.writeMicroseconds(Wengage);
          break;
        case 'w':
          servoW.writeMicroseconds(Wdisengage);
          break;
        case 'N':
          servoN.writeMicroseconds(Nengage);
          break;
        case 'n':
          servoN.writeMicroseconds(Ndisengage);
          break;
        case 'S':
          servoS.writeMicroseconds(Sengage);
          break;
        case 's':
          servoS.writeMicroseconds(Sdisengage);
          break;
      }
    if(str[i] == ' ' || i == len)
      delay(timeDelay);
    start = i+1;
    }
  }
}

void setup() {
  //pwm, and normal digital pins all work on the arduion mega. Digital pins on arduino uno seems to cause issues (or my uno is bad)
  servoR.attach(51); servoL.attach(23); servoU.attach(49); servoD.attach(25);
  servoE.attach(50); servoW.attach(22); servoN.attach(48); servoS.attach(24);
  Serial.begin(9600);
  //set servos to default values | 0 for rotating servos, disengage for engage servos
  servoR.writeMicroseconds(500); servoL.writeMicroseconds(500); servoU.writeMicroseconds(500); servoD.writeMicroseconds(500);
  servoE.write(Edisengage); servoW.write(Wdisengage); servoN.write(Ndisengage); servoS.write(Sdisengage);

  char testStrOne[] = "R0.L0.U0.D0 e.w.n.s N.S.W E U2.D2 n.e.w R1 E.W s R0.L1 S e.w U0.L0 N.E.W U2.D0 R2.L2 e.w.n.s";
  char testStrTwo[] = "R0.L0.U0.D0 e.w.n.s s s N.S.W E N.E.S.W.U0.R0.D2.L0 n.e.S.w.U0.R0.D2.L0 n.e.S.w.U1.R1.D2.L0 n.E.S.W.U2.R1.D2.L0 n.E.s.W.U1.R1.D2.L0 n.E.s.W.U0.R0.D2.L1 n.E.S.W.U0.R0.D2.L1 N.e.S.w.U0.R0.D2.L1 N.e.S.w.U1.R0.D1.L2 n.E.S.W.U1.R0.D1.L2 n.E.S.W.U0.R0.D2.L2 n.e.S.w.U1.R0.D2.L2 n.e.S.w.U2.R1.D2.L2 N.E.S.W.U2.R1.D2.L2 N.E.S.W.U2.R2.D2.L2 N.E.S.W.U1.R2.D2.L2 n.e.S.w.U1.R2.D2.L2 n.e.S.w.U1.R1.D2.L2 n.E.S.W.U0.R1.D2.L2 n.E.s.W.U1.R1.D2.L2 n.E.s.W.U0.R2.D2.L1 n.E.S.W.U1.R2.D2.L1 n.e.S.w.U0.R2.D2.L1 n.e.S.w.U0.R2.D2.L0 N.E.S.W.U0.R2.D2.L0 N.E.S.W.U0.R2.D0.L0 N.E.S.W.U0.R2.D0.L2 N.E.S.W.U0.R2.D1.L2 N.e.S.w.U0.R2.D1.L2 N.e.S.w.U1.R1.D0.L1 n.e.S.w.U1.R1.D0.L0 n.E.S.W.U2.R1.D0.L0 n.E.s.W.U2.R1.D0.L0 n.E.s.W.U2.R0.D0.L1 N.E.S.W.U2.R0.D0.L1 N.E.S.W.U2.R0.D0.L0 n.E.S.W.U2.R0.D1.L0 n.E.S.W.U1.R0.D1.L0 N.e.S.w.U1.R0.D1.L0 N.e.S.w.U0.R1.D2.L0 N.E.S.W.U0.R1.D2.L0 N.E.S.W.U0.R2.D2.L0 n.E.S.W.U0.R2.D1.L0 n.E.S.W.U1.R2.D1.L0 N.e.S.w.U1.R2.D1.L0 N.e.S.w.U0.R1.D2.L0 N.E.S.W.U0.R1.D2.L0 N.E.S.W.U0.R0.D2.L0 N.E.S.W.U1.R0.D2.L0 n.e.S.w.U1.R0.D2.L0 n.e.S.w.U1.R0.D2.L1 n.E.S.W.U0.R0.D2.L1 n.E.s.W.U1.R0.D2.L1 n.E.s.W.U0.R1.D2.L0 n.E.S.W.U0.R1.D2.L0 N.e.S.w.U0.R1.D2.L0 N.e.S.w.U1.R0.D1.L0 N.E.S.W.U1.R0.D1.L0 N.E.S.W.U1.R0.D0.L0 n.e.S.w.U1.R0.D0.L0 n.e.S.w.U0.R1.D0.L0 N.E.S.W.U0.R1.D0.L0 N.E.S.W.U0.R2.D0.L0 n.E.S.W.U0.R2.D1.L0 n.E.S.W.U1.R2.D1.L0 N.e.S.w.U1.R2.D1.L0 N.e.S.w.U0.R1.D2.L0 n.E.S.W.U0.R1.D2.L0 n.E.s.W.U1.R1.D2.L0 n.E.s.W.U0.R0.D2.L1 N.E.S.W.U0.R0.D2.L1 N.E.S.W.U0.R0.D2.L0 N.E.S.W.U2.R0.D2.L0 N.E.S.W.U2.R1.D2.L1 n.e.S.w.U2.R1.D2.L1 n.e.S.w.U2.R2.D2.L0 N.E.S.W.U2.R2.D2.L0 N.E.S.W.U1.R2.D2.L0 n.e.S.w.U1.R2.D2.L0 n.e.S.w.U2.R1.D2.L0 N.E.S.W.U2.R1.D2.L0 N.E.S.W.U2.R0.D2.L0 N.E.S.W.U0.R0.D2.L0 N.E.S.W.U0.R1.D2.L0 n.e.S.w.U0.R1.D2.L0 n.e.S.w.U1.R2.D2.L0 N.E.S.W.U1.R2.D2.L0 N.E.S.W.U2.R2.D2.L0 n.e.S.w.U2.R2.D2.L0 n.e.S.w.U1.R2.D2.L1 n.E.S.W.U0.R2.D2.L1 n.E.s.W.U1.R2.D2.L1 n.E.s.W.U0.R1.D2.L2 n.E.S.W.U1.R1.D2.L2 n.e.S.w.U0.R1.D2.L2 n.e.S.w.U0.R2.D2.L2 N.E.S.W.U0.R2.D2.L2 N.E.S.W.U1.R2.D2.L2 n.e.S.w.U1.R2.D2.L2 n.e.S.w.U1.R2.D2.L1 n.E.S.W.U0.R2.D2.L1 n.E.s.W.U1.R2.D2.L1 n.E.s.W.U0.R1.D2.L2 n.E.S.W.U0.R1.D2.L2 N.e.S.w.U0.R1.D2.L2 N.e.S.w.U1.R0.D1.L2 n.E.S.W.U1.R0.D1.L2 n.E.S.W.U2.R0.D0.L2 N.E.S.W.U2.R0.D0.L2 N.E.S.W.U2.R0.D0.L1 N.e.S.w.U2.R0.D0.L1 N.e.S.w.U1.R0.D1.L0 N.E.S.W.U1.R0.D1.L0 N.E.S.W.U1.R0.D0.L0 n.e.S.w.U1.R0.D0.L0 n.e.S.w.U2.R1.D0.L0 N.E.S.W.U2.R1.D0.L0 N.E.S.W.U2.R0.D0.L0 N.E.S.W.U2.R0.D1.L0 e.w.n.s";
  int count = 0;
  for(; testStrTwo[count]; count++){}
  execute(testStrTwo, count);
}

void loop() {
  //code to get search for string
  /*if (Serial.available() > 0) {
    String data = Serial.read();
    Serial.print("You sent me: ");
    Serial.println(data);
  }*/
  char byte = 0;
  Serial.readBytes(&byte, 1);
  Serial.print(byte);
  if (byte == 'R'){
    servoR.writeMicroseconds(1200);
  }
  if (byte == 'L'){
    servoR.writeMicroseconds(500);
  }
  if (byte == 'O'){
    servoR.writeMicroseconds(1900);
  }
  if (byte == 'e')
  {
    servoE.writeMicroseconds(Edisengage);
  }
  if (byte == 'E'){
    servoE.writeMicroseconds(Eengage);
  }
    if (byte == 'w')
  {
    servoW.writeMicroseconds(Wdisengage);
  }
  if (byte == 'W'){
    servoW.writeMicroseconds(Wengage);
  }
    if (byte == 'n')
  {
    servoN.writeMicroseconds(Ndisengage);
  }
  if (byte == 'N'){
    servoN.writeMicroseconds(Nengage);
  }
    if (byte == 's')
  {
    servoS.writeMicroseconds(Sdisengage);
  }
  if (byte == 'S'){
    servoS.writeMicroseconds(Sengage);
  }
}
