#include <Servo.h> 
Servo servoR, servoL, servoU, servoD, servoRE, servoLE, servoUE, servoDE;
//https://docs.arduino.cc/learn/electronics/servo-motors

#define timeDelay 5000 //in miliseconds
#define engageAngle 5000

struct command{
  int length;
  Servo servo[];
  int angle[];
};

//0-180 for all servos is clockwise, U, R, L, D | 180-0 is U', R', L', D'
//EngageAngle-0 For Right+UP engage servos is Engage (RE or UE), 0-EngageAngle is Disengage (RE' or UE')
//0-EngageAngle for Left+Down engage servos is Engage (LE or DE), EngageAngle-0 is Disengage (LE' or DE')

void execute(char str[], int len, int cmdAmnt){
  command commands[cmdAmnt];
  int start = 0, cmd = 0, simulCount = 0;
  for(int i = 0; i < len; i++){
    if(str[i] == ' ' || str[i] == '.'){
      switch (str[start]) {
        case '1':
          commands[cmd].servo[simulCount] = servoR;
          break;
        case '2':
          commands[cmd].servo[simulCount] = servoL;
          break;
        case '3':
          commands[cmd].servo[simulCount] = servoU;
          break;
        case '4':
          commands[cmd].servo[simulCount] = servoD;
          break;
        case '5':
          commands[cmd].servo[simulCount] = servoRE;
          break;
        case '6':
          commands[cmd].servo[simulCount] = servoLE;
          break;
        case '7':
          commands[cmd].servo[simulCount] = servoUE;
          break;
        case '8':
          commands[cmd].servo[simulCount] = servoDE;
          break;
        start++;
        int angleLength = i - start;
        String angle;
        for(int i = 0; i < angleLength; i++)
          angle[i] = str[start+i];
        angle[angleLength] = '\0';
        commands[cmd].angle[simulCount] = angle.toInt();
        if(str[i] == ' '){
          commands[cmd].length = simulCount+1;
          cmd++;
          simulCount = 0;
        }
        else
          simulCount++;
        start = i+1;
      }
    }
  }
  for(int i = 0; i < cmdAmnt; i++){
    for(int j = 0; j < commands[i].length; j++)
      commands[i].servo[j].write(commands[i].angle[j]);
    delay(timeDelay);
  }
}

void setup() {
  //pwm, and normal digital pins all work for this
  servoR.attach(22); servoL.attach(23); servoU.attach(24); servoD.attach(25);
  servoRE.attach(28); servoLE.attach(29); servoUE.attach(30); servoDE.attach(31);
  Serial.begin(9600);
  //set servos to default values
  servoR.write(0); servoL.write(0); servoU.write(0); servoD.write(0);
  servoRE.write(engageAngle); servoLE.write(0); servoUE.write(engageAngle); servoDE.write(0);
}

void loop() {
  //test string "U.RE'.LE' R2.L' RE.LE"
  char str[] = "1180.290 30 490.5144.60";
  int count, cmdAmnt = 0;
  for(int i  = 0; str[i]; i++){
    count = i;
    if(str[i] == ' ')
      cmdAmnt++;
  }
  execute(str, count, cmdAmnt);

  /*char byte = 0;
  Serial.readBytes(&byte, 1);
  Serial.print(byte);
  if (byte == 'R'){
    servoR.write(180);
    servoL.write(0);
  }
  if (byte == 'L'){
    servoR.write(0);
    servoL.write(180);
  }
  if (byte == 'O'){
    servoR.write(90);
    servoL.write(90);
  }*/

}
