#include <Servo.h> 
//Subscript indicates Right, Left, Up, Down for the turning servos, and East, West, North, and South for the engage servos all relative to the camera
Servo servoR, servoL, servoU, servoD, servoE, servoW, servoN, servoS;
//https://docs.arduino.cc/learn/electronics/servo-motors

#define timeDelay 5000 //in miliseconds
#define engageAngle 144 //max angle that engage servos go to
#define halfTurn 500 //in Milliseconds
#define quarterTurn 350 //in Milliseconds
#define engageTime 375 //in Milliseconds


//0-180 for all servos is clockwise | 180-0 is counter clockwise
//0 For East+North servos is Engage (capital char) | engageAngle is Disengage (lowercase char)
//engageAngle For West+South servos is Engage (capital char) | 0 is Disengage (lowercase char)

void execute(char str[], int len){
  int start = 0, angle;
  for(int i = 0; i <= len; i++){
    if(str[i] == ' ' || str[i] == '.' || i == len){
      angle = (str[start+1] - '0')*90;
      switch (str[start]) {
        case 'R':
          servoR.write(angle);
          break;
        case 'L':
          servoL.write(angle);
          break;
        case 'U':
          servoU.write(angle);
          break;
        case 'D':
          servoD.write(angle);
          break;
        case 'E':
          servoE.write(0);
          break;
        case 'e':
          servoE.write(engageAngle);
          break;
        case 'W':
          servoW.write(engageAngle);
          break;
        case 'w':
          servoW.write(0);
          break;
        case 'N':
          servoN.write(0);
          break;
        case 'n':
          servoN.write(engageAngle);
          break;
        case 'S':
          servoS.write(engageAngle);
          break;
        case 's':
          servoS.write(0);
          break;
      }
    if(str[i] == ' ' || i == len)
      delay(timeDelay);
    start = i+1;
    }
  }
}

void setup() {
  //pwm, and normal digital pins all work for this
  servoR.attach(22); servoL.attach(23); servoU.attach(24); servoD.attach(25);
  servoE.attach(28); servoW.attach(29); servoN.attach(30); servoS.attach(31);
  Serial.begin(9600);
  //set servos to default values | 0 for rotating servos, disengage for engage servos
  servoR.write(0); servoL.write(0); servoU.write(0); servoD.write(0);
  servoE.write(engageAngle); servoW.write(0); servoN.write(engageAngle); servoS.write(0);

  char testStr[] = "R0.L0.U0.D0.e.w.n.s E.W.N.S"; //"R0.L0.U0.D0.e.w.n.s R2.L2.U2.D2 R1.L1.U1.D1 E.W.N.S e.w.n.s";//"R2.L2 R0.E R1.e";
  int count = 0;
  for(; testStr[count]; count++){}
  execute(testStr, count);
}

void loop() {
  //code to get search for string
  /*then:
  int count = 0;
  for(; testStr[count]; count++){}
  execute(testStr, count);
  */
  char byte = 0;
  Serial.readBytes(&byte, 1);
  Serial.print(byte);
  if (byte == 'R'){
    servoL.write(0);
    //servoL.writeMicroseconds(1280);
  }
  if (byte == 'L'){
    servoL.write(144);
  }
  if (byte == 'O'){
    servoL.writeMicroseconds(2000);
  }
  if (byte == 'e')
  {
    servoL.write(engageAngle);
  }
  if (byte == 'E'){
    servoL.write(45);
  }
}
