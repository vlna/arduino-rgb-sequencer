//    This is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    Foobar is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

// control sample
//////////////////////////////
// F - fade up/down time - channel independent
// L - loops - channel independent 
// RGB control chanel
// 12345678 intensity
// S start
// E end
// C cycle length
// P load preset
// ? show program

#include <SoftPWM.h>

#define RED 2
#define GREEN 3
#define BLUE 4

// COMMON_CATHODE for common GND on RGB diode
// COMMON_ANODE   for common VCC on RGB diode

//#define COMMON_CATHODE
#define COMMON_ANODE

#ifdef COMMON_CATHODE
  #ifdef COMMON_ANODE
    #error "COMMON_CATHODE and COMMON_ANODE cannot be defined together."
  #endif
#endif


byte intensityR[8] = {  0, 255,   0, 255,   0, 255,   0, 255};
byte intensityG[8] = {255,   0, 255,   0, 255,   0, 255,   0};
byte intensityB[8] = {  0,   0, 255, 255,   0,   0, 255, 255};

int fade[8] = {2000, 2000, 2000, 2000, 2000, 2000, 2000, 2000};

int loops = 9999; 

byte pointer = 0;
byte steps = 8;

boolean run = true;

unsigned long previousMillis = 0;
unsigned long currentMillis;
 
String inputString;

void setup() {
  Serial.begin(9600);

  #ifdef COMMON_CATHODE
    SoftPWMBegin(SOFTPWM_NORMAL);
  #endif  
  #ifdef COMMON_ANODE
    SoftPWMBegin(SOFTPWM_INVERTED);
  #endif  
  SoftPWMSet(RED,   intensityR[0]);
  SoftPWMSet(GREEN, intensityG[0]);
  SoftPWMSet(BLUE,  intensityB[0]);

  SoftPWMSetFadeTime(RED,   fade[0], fade[0]);
  SoftPWMSetFadeTime(GREEN, fade[0], fade[0]);
  SoftPWMSetFadeTime(BLUE,  fade[0], fade[0]);

  delay(fade[0]);
}

void loop() {
  if ( (loops != 0) && run) {
    if (loops<9999) { loops--; }
    pointer++;
    if (pointer >= steps) {pointer = 0;}
    Serial.println(pointer);

    SoftPWMSetFadeTime(RED,   fade[pointer], fade[pointer]);
    SoftPWMSetFadeTime(GREEN, fade[pointer], fade[pointer]);
    SoftPWMSetFadeTime(BLUE,  fade[pointer], fade[pointer]);

    SoftPWMSet(RED,   intensityR[pointer]);
    SoftPWMSet(GREEN, intensityG[pointer]);
    SoftPWMSet(BLUE,  intensityB[pointer]);

    //delay(fade[pointer]);
    previousMillis = millis();
    currentMillis = millis();
    
    while (currentMillis - previousMillis <= fade[pointer]) {
      serialEvent();
      currentMillis = millis();
    }
  }
  serialEvent();
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      processInput();
      inputString = "";
    }
  }
}

void processInput() {
  if (inputString.length() > 0 && inputString[0]=='E') { run = false; serialOK(); }
  if (inputString.length() > 0 && inputString[0]=='S') { run = true;  serialOK(); }

  if (inputString.length() > 0 && inputString[0]=='?') { showProgram(); }

  if (inputString[0]=='L' && inputString.length() > 1) {
     loops = inputString.substring(1).toInt();
     serialOK();
    }

  if (inputString[0]=='F' && inputString[1]>='0' && inputString[1]<='7' && inputString.length() > 2 ) {
    fade[inputString.substring(1,2).toInt()] = constrain(inputString.substring(2).toInt(),0,4000);
    serialOK();
  }

  if (inputString[0]=='R' && inputString[1]>='0' && inputString[1]<='7' && inputString.length() > 2 ) {
    intensityR[inputString.substring(1,2).toInt()] = constrain(inputString.substring(2).toInt(),0,255);
    serialOK();
  }
  if (inputString[0]=='G' && inputString[1]>='0' && inputString[1]<='7' && inputString.length() > 2 ) {
    intensityG[inputString.substring(1,2).toInt()] = constrain(inputString.substring(2).toInt(),0,255);
    serialOK();
  }
  if (inputString[0]=='B' && inputString[1]>='0' && inputString[1]<='7' && inputString.length() > 2 ) {
    intensityB[inputString.substring(1,2).toInt()] = constrain(inputString.substring(2).toInt(),0,255);
    serialOK();
  }

  if (inputString[0]=='P' && inputString[1]>='0' && inputString[1]<='7' && inputString.length() > 1 ) {
    preset(inputString.substring(1,2).toInt());
    serialOK();
  }

  if (inputString[0]=='C' && inputString[1]>='1' && inputString[1]<='7' && inputString.length() > 1 ) {
    steps = inputString.substring(1,2).toInt();
    serialOK();
  }
}

void serialOK() {
  Serial.println("OK");
}

void preset(byte presetNo) {
  switch (presetNo) {
    case 0:
       steps = 8;
       for (int i = 0; i < steps; i++) {
         intensityR[i] = (i & 1) * 255 ;
         intensityG[i] = (~i & 1) * 255;
         intensityB[i] = ((i >> 1) & 1) * 255 ;
         fade[i]       = 2000;
       }
       loops = 9999; 
       pointer = 0;
       break;
    case 1:
       steps = 6;
       for (int i = 0; i < steps; i++) {
         intensityR[i] = i==1?255:0 ;
         intensityG[i] = i==3?255:0;
         intensityB[i] = i==5?255:0 ;
         fade[i]       = 200;
       }
       loops = 9999; 
       pointer = 0;
       break;
    case 2:
       steps = 3;
       for (int i = 0; i < steps; i++) {
         intensityR[i] = i==0?255:0 ;
         intensityG[i] = i==1?255:0;
         intensityB[i] = i==2?255:0 ;
         fade[i]       = 200;
       }
       loops = 9999; 
       pointer = 0;
       break;
    //TODO case 3-5
    case 6:
       steps = 1;
       for (int i = 0; i < steps; i++) {
         intensityR[i] = 0 ;
         intensityG[i] = 0;
         intensityB[i] = 0 ;
         fade[i]       = 4000;
       }
       loops = 9999; 
       pointer = 0;
       break;

    case 7:
       steps = 1;
       for (int i = 0; i < steps; i++) {
         intensityR[i] = 255;
         intensityG[i] = 255;
         intensityB[i] = 255;
         fade[i]       = 4000;
       }
       loops = 9999; 
       pointer = 0;
       break;

  }
}

void showProgram() {
  Serial.write("Red:   ");
  for (int i = 0; i < 8; i++) { Serial.print(intensityR[i]); Serial.write(',');; }
  Serial.println(); 
  Serial.write("Green: ");
  for (int i = 0; i < 8; i++) { Serial.print(intensityG[i]); Serial.write(','); }
  Serial.println(); 
  Serial.write("Blue:  ");
  for (int i = 0; i < 8; i++) { Serial.print(intensityB[i]); Serial.write(','); }
  Serial.println(); 
  Serial.write("Fade:  ");
  for (int i = 0; i < 8; i++) { Serial.print(fade[i]); Serial.write(','); }
  Serial.println(); 

  Serial.write("Steps: ");
  Serial.println(steps);

  Serial.write("Loops: ");
  Serial.println(loops);

  Serial.print("Prg is ");
  if (!run) {Serial.print("not");}
  Serial.println(" running.");
}
