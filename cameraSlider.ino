#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <WiFi.h>
#include <BlynkSimpleEnergiaWiFi.h>

WidgetTerminal terminal ( V10 );

// auth token from Blynk app
char auth[] = "pCwrUm7pZlOar0TwYamRukuKFblYcWOh";

// wifi credentials
char ssid[] = "Central_Perk";
char pass[] = "Unagi9896";

int totalLength = 690; // length of slider in mm
double travelTime; // time for camera to travel down slider

// steps to move along the entire rail
// 400 * ( totalLength / ( 3.14 * pulleyDiameter ) )
// pulleyDiameter = 12.5
double totalSteps = 7031 * 3.37;

double stepDelay; // delay for each step -> used for speed
int stepsTaken = 0; // number of steps taken -> count to stop before end of slider

// control variables
bool dir; // direction
bool turn; // enable/disable turning of stepper
bool rst;  // reset
bool manual;  // direction for moving without timer

// switch state variables
bool s3;
bool s4;
bool s5;
bool s6;

// pins for stepper motor control
const int stepPin = 8; // step pin
const int dirPin = 7; // direction pin
const int ms1 = 26;  // microstep 1
const int ms2 = 27;  // microstep 2
const int ms3 = 28;  // microstep 3


void setup() {

  // enable serial moitor
  Serial.begin ( 115200 );

  // start Blynk connection using wifi credentials
  Blynk.begin ( auth, ssid, pass ); 

  terminal.clear(); // clear terminal on Blynk app

  // set stepper motor outputs
  pinMode ( stepPin, OUTPUT );
  pinMode ( dirPin, OUTPUT );
  pinMode ( ms1, OUTPUT );
  pinMode ( ms2, OUTPUT );
  pinMode ( ms3, OUTPUT );

  // print into to Blynk terminal
  terminal.println ( "WI-SLI V1" );
  terminal.println ( "WI-FI ENABLED CAMERA SLIDER" );
  terminal.println ( "" );
  terminal.println ( "MODES:" );
  terminal.println ( "0000 -> OFF" );
  terminal.println ( "0001 -> 20 SEC" );
  terminal.println ( "0010 -> 25 SEC" );
  terminal.println ( "0011 -> 30 SEC" );
  terminal.println ( "0100 -> 1 MIN" );
  terminal.println ( "0101 -> 2 MIN" );
  terminal.println ( "0110 -> 5 MIN" );
  terminal.println ( "0111 -> 20 MIN" );
  terminal.println ( "1000 -> 30 MIN" );
  terminal.println ( "1001 -> 40 MIN" );
  terminal.println ( "1010 -> 50 MIN" );
  terminal.println ( "1011 -> 1 HR" );
  terminal.println ( "1100 -> 1 HR 30 MIN" );
  terminal.println ( "1101 -> 4 HRS" );
  terminal.println ( "1110 -> 8 HR" );
  terminal.println ( "1111 -> 12 HR" );

  terminal.flush ();  // make sure all commands sent to Blynk app terminal

  digitalWrite ( dirPin, true );  // set inital direction to move camera left

  // enable 1/16th microstepping
  digitalWrite ( ms1, true );
  digitalWrite ( ms2, true );
  digitalWrite ( ms3, true );
}

// Virtual Pin 2 -> button for reset
BLYNK_WRITE ( V2 ) {  
  rst = param.asInt();
}

// Virtual Pin 3 -> switch 1 for speed
BLYNK_WRITE ( V3 ) {  
  s3 = param.asInt();
}

// Virtual Pin 4 -> switch 2 for speed
BLYNK_WRITE ( V4 ) {  
  s4 = param.asInt(); 
}

// Virtual Pin 5 -> switch 3 for speed
BLYNK_WRITE ( V5 ) {  
  s5 = param.asInt(); 
}

// Virtual Pin 6 -> switch 4 for speed
BLYNK_WRITE ( V6 ) {  
  s6 = param.asInt(); 
}

// Virtual Pin 7 -> switch to change direction
BLYNK_WRITE ( V7 ) {  
  dir = param.asInt();
}

// Virtual Pin 12 -> manually move stepper
BLYNK_WRITE ( V12 ) {  
  manual = param.asInt ();
}

// move stepper forward by 1 step
void rotateStepper () {
  digitalWrite ( stepPin, true );
  delayMicroseconds ( 400 );
  digitalWrite ( stepPin, false );
  delayMicroseconds ( 400 );
  stepsTaken++; // update total number of steps taken by 1
}

// calculates the delay required in ms between steps
int delayCalc ( int travelTime ) {
  // 0.0008 Seconds are deducted because the rotateStepper includes 2x 400 microseconds delays
  return ( ( travelTime / totalSteps ) - 0.0008 ) * 1000;
}

void loop() {

  // run Blynk app
  Blynk.run();

  if ( dir == 1 ) {
    // move camera left
    digitalWrite ( dirPin, true );
  }
  else {
    // move camera right
    digitalWrite ( dirPin, false );
  }

  // if reset in Blynk is pressed, set everything back to 0
  if ( rst ) {
    stepsTaken = 0;
    travelTime = 0;
    terminal.println ( "RESET TO 0" );
    rst = 0;
  }

  // manually move stepper using the Blynk app
  if ( manual ) {
    rotateStepper ();
  }

  // choose length of slide based on the combination of switches on Blynk app
  // s3 s4 s5 s6 -> 16 possible options
  // 0000 -> stationary
  // 0001  -> 10s
  if ( ( s3 == false ) && ( s4 == false ) && ( s5 == false ) && ( s6 == true ) ) {
    if ( stepsTaken < totalSteps ) {
      travelTime = 10;  // time for slider to move
      stepDelay = delayCalc ( travelTime );
      rotateStepper();
      delay ( stepDelay );
    }
  }
  // 0010 -> 20s
  if ( ( s3 == false ) && ( s4 == false ) && ( s5 == true ) && ( s6 == false ) ) {
    if ( stepsTaken < totalSteps ) {
      travelTime = 20;  // time for slider to move
      stepDelay = delayCalc ( travelTime );
      rotateStepper();
      delay ( stepDelay );
    }
  }
  // 0011 -> 30s
  if ( ( s3 == false ) && ( s4 == false ) && ( s5 == true ) && ( s6 == true ) ) {
    if ( stepsTaken < totalSteps ) {
      travelTime = 30;  // time for slider to move
      stepDelay = delayCalc ( travelTime );
      rotateStepper();
      delay ( stepDelay );  
    }
  }
  // 0100 -> 60s
  if ( ( s3 == false ) && ( s4 == true ) && ( s5 == false ) && ( s6 == false ) ) {
    if ( stepsTaken < totalSteps ) {
      travelTime = 60;  // time for slider to move
      stepDelay = delayCalc ( travelTime );
      rotateStepper();
      delay ( stepDelay );  
    }
  }
  // 0101 -> 120s
  if ( ( s3 == false ) && ( s4 == true ) && ( s5 == false ) && ( s6 == true ) ) {
    if ( stepsTaken < totalSteps ) {
      travelTime = 120;  // time for slider to move
      stepDelay = delayCalc ( travelTime );
      rotateStepper();
      delay ( stepDelay );  
    }
  }
  // 0110 -> 300s
  if ( ( s3 == false ) && ( s4 == true ) && ( s5 == true ) && ( s6 == false) ) {
    if ( stepsTaken < totalSteps ) {
      travelTime = 300;  // time for slider to move
      stepDelay = delayCalc ( travelTime );
      rotateStepper();
      delay ( stepDelay );  
    }
  }
  // 0111 -> 1200s
  if ( ( s3 == false ) && ( s4 == true ) && ( s5 == true ) && ( s6 == true ) ) {
    if ( stepsTaken < totalSteps ) {
      travelTime = 1200;  // time for slider to move
      stepDelay = delayCalc ( travelTime );
      rotateStepper();
      delay ( stepDelay );  
    }
  }
  // 1000 -> 1800s
  if ( ( s3 == true ) && ( s4 == false ) && ( s5 == false ) && ( s6 == false ) ) {
    if ( stepsTaken < totalSteps ) {
      travelTime = 1800;  // time for slider to move
      stepDelay = delayCalc ( travelTime );
      rotateStepper();
      delay ( stepDelay );  
    }
  }
  // 1001 -> 2400s
  if ( ( s3 == true ) && ( s4 == false ) && ( s5 == false ) && ( s6 == true ) ) {
    if ( stepsTaken < totalSteps ) {
      travelTime = 2400;  // time for slider to move
      stepDelay = delayCalc ( travelTime );
      rotateStepper();
      delay ( stepDelay );  
    }
  }
  // 1010 -> 3000s
  if ( ( s3 == true ) && ( s4 == false ) && ( s5 == true ) && ( s6 == false ) ) {
    if ( stepsTaken < totalSteps ) {
      travelTime = 3000;  // time for slider to move
      stepDelay = delayCalc ( travelTime );
      rotateStepper();
      delay ( stepDelay );  
    }
  }
  // 1011 -> 3600s
  if ( ( s3 == true ) && ( s4 == false ) && ( s5 == true ) && ( s6 == true ) ) {
    if ( stepsTaken < totalSteps ) {
      travelTime = 3600;  // time for slider to move
      stepDelay = delayCalc ( travelTime );
      rotateStepper();
      delay ( stepDelay );  
    }
  }
  // 1100 -> 5400s
  if ( ( s3 == true ) && ( s4 == true ) && ( s5 == false ) && ( s6 == false ) ) {
    if ( stepsTaken < totalSteps ) {
      travelTime = 5400;  // time for slider to move
      stepDelay = delayCalc ( travelTime );
      rotateStepper();
      delay ( stepDelay );  
    }
  }
  // 1101 -> 14400s
  if ( ( s3 == true ) && ( s4 == true ) && ( s5 == false ) && ( s6 == true ) ) {
    if ( stepsTaken < totalSteps ) {
      travelTime = 14400;  // time for slider to move
      stepDelay = delayCalc ( travelTime );
      rotateStepper();
      delay ( stepDelay );  
    }
  }
  // 1110 -> 28800s
  if ( ( s3 == true ) && ( s4 == true ) && ( s5 == true ) && ( s6 == false ) ) {
    if ( stepsTaken < totalSteps ) {
      travelTime = 14400;  // time for slider to move
      stepDelay = delayCalc ( travelTime );
      rotateStepper();
      delay ( stepDelay );  
    }
  }
  // 1111 -> 43200s
  if ( ( s3 == true ) && ( s4 == true ) && ( s5 == true ) && ( s6 == true ) ) {
    if ( stepsTaken < totalSteps ) {
      travelTime = 14400;  // time for slider to move
      stepDelay = delayCalc ( travelTime );
      rotateStepper();
      delay ( stepDelay );  
    }
  }
  Serial.println ( stepsTaken );
}
