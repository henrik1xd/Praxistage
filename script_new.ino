#include <PS2X_lib.h>  //for v1.6
#include <Adafruit_MotorShield.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

  #define Gyro_SCL 5
  #define Gyro_XDA 4
  #define Ultraschall_TriggerPin 46
  #define Ultraschall_EchoPin 48
  #define Gamepad_DI 22
  #define Gamepad_D0 23
  #define Gamepad_CS 24
  #define Gamepad_CLK 25
  #define Line_r_A0 0
  #define Line_l_A1 1
  #define Turn_vr_out 2
  #define Turn_hr_out 3
  #define Turn_vl_out 18
  #define Turn_hl_out 19
  #define Color_LED 51
  #define Color_S3 45
  #define Color_S2 47
  #define Color_out 49
  #define Servo_Rechts  125
  #define Servo_mitte  320
  #define Servo_Links  515
  #define Servo_Freq 50

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *Motor_1 = AFMS.getMotor(1);
Adafruit_DCMotor *Motor_2 = AFMS.getMotor(2);
Adafruit_DCMotor *Motor_3 = AFMS.getMotor(3);
Adafruit_DCMotor *Motor_4 = AFMS.getMotor(4);

PS2X ps2x; // create PS2 Controller Class

//Variablen Definitionen===============================================================================================================================================================================
  int error = 0; 
  int speedy = 100;

  byte type = 0;
  byte vibrate = 0;

  unsigned long Zeit_Ultraschallsensor, Zeit_Servo, Zeit_AblaufStation14, Zeit_Station14BoxenRegistrieren;

//Variablen Station5===============================================================================
  int activateStation5 = 0; //0=false, 1=true
  float umfang = 3.14159265359*0.065; //in Meter
  float distancePerSchranke = umfang/20;
  float n1, n2, m1, m2, distance, Station5_Abstand1, Station5_Abstand2, Station5_Winkel1, Station5_Winkel2;
  int U1, U2, U3;

//Variablen Station14==============================================================================
  float Ultraschall_Distance, Box1_MinEntfernung, Box2_MinEntfernung;
  bool Servo_Langsam, Start_AblaufStation14, Station14_RegistriereBoxen, Station14_BoxenRegistriert, Station14_Messung;
  uint16_t Servo_PulsIst, Servo_PulsSoll;
  unsigned long Zeit_Laufzeit;



//SetUp================================================================================================================================================================================================
  void setup(){
    //Pinbelegung==================================================================================
      pinMode(Ultraschall_TriggerPin, OUTPUT);
      pinMode(Ultraschall_EchoPin, INPUT);

    //Serielle Schnittstelle=======================================================================
      Serial.begin(9600);

    //Servo========================================================================================    
      pwm.begin();
      pwm.setOscillatorFrequency(27000000);
      pwm.setPWMFreq(Servo_Freq);

    //Radmotor=====================================================================================
      AFMS.begin(); // create with the default frequency 1.6KHz
      Motor_1->setSpeed(255);
      Motor_1->run(BACKWARD);
      Motor_1->run(FORWARD);
      Motor_1->run(RELEASE);

      Motor_2->setSpeed(255);
      Motor_2->run(BACKWARD);
      Motor_2->run(FORWARD);
      Motor_2->run(RELEASE);

      Motor_3->setSpeed(255);
      Motor_3->run(BACKWARD);
      Motor_3->run(FORWARD);
      Motor_3->run(RELEASE);

      Motor_4->setSpeed(255);
      Motor_4->run(BACKWARD);
      Motor_4->run(FORWARD);
      Motor_4->run(RELEASE);

    //Gamepad======================================================================================
      error = ps2x.config_gamepad(25,23,24,22, true, true);   //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
      type = ps2x.readType();

    //Pause========================================================================================
      delay(10);
  
}

//Henriks Funktion=====================================================================================================================================================================================
  bool Motor(String Motor ,int direction = 0,int speed = speedy){
    
    if (Motor == "hr") {
      Motor_1->setSpeed(speed);
      if (direction == 0) {Motor_1->run(RELEASE);}
      else if (direction == 1) {Motor_1->run(FORWARD);}
      else if (direction == 2) {Motor_1->run(BACKWARD);}
      return true;
    }
    else if (Motor == "hl") {
      Motor_4->setSpeed(speed);
      if (direction == 0) {Motor_4->run(RELEASE);}
      else if (direction == 1) {Motor_4->run(FORWARD);}
      else if (direction == 2) {Motor_4->run(BACKWARD);} 
      return true;
    }
    else if (Motor == "vr") {
      Motor_2->setSpeed(speed);
      if (direction == 0) {Motor_2->run(RELEASE);}
      else if (direction == 1) {Motor_2->run(FORWARD);}
      else if (direction == 2) {Motor_2->run(BACKWARD);}
      return true;
    }
    else if (Motor == "vl") {
      Motor_3->setSpeed(speed);
      if (direction == 0) {Motor_3->run(RELEASE);}
      else if (direction == 1) {Motor_3->run(FORWARD);}
      else if (direction == 2) {Motor_3->run(BACKWARD);} 
      return true;
    }
  }


//blink Funktion=======================================================================================================================================================================================
  void blink() {
    distance += distancePerSchranke;
  }
  void DrehungGegenUhr(float n,float a) {
    if (a < 0) {
      DrehungInUhr(n, abs(a));
      return;
    }
    float time = (700/90)*a*n;
    Motor("vr",1); 
    Motor("vl",2);  
    Motor("hr",1);  
    Motor("hl",2);
    delay(time);
    Motor("vr",0); 
    Motor("vl",0);  
    Motor("hr",0);  
    Motor("hl",0);
    return;
  }
  void DrehungInUhr(float n,float a) {
    if (a < 0) {
      DrehungGegenUhr(n, abs(a));
      return;
    }
    float time = (700/90)*a*n;
    Motor("vr",2); 
    Motor("vl",1);  
    Motor("hr",2);  
    Motor("hl",1);
    delay(time);
    Motor("vr",0); 
    Motor("vl",0);  
    Motor("hr",0);  
    Motor("hl",0);
    return;
  }


//Station5=============================================================================================================================================================================================
  bool Station5(float x1,float x2,float a1,float a2) {

    
    //Eingabe der Parameter:
    if(ps2x.ButtonPressed(PSB_L3))  {          //will be TRUE if button was JUST pressed
        U1 += 1;
      }
    if(ps2x.ButtonPressed(PSB_R3))  {          //will be TRUE if button was JUST pressed
        U2 += 1;
      }
    if(ps2x.ButtonPressed(PSB_PINK))  {          //will be TRUE if button was JUST pressed
        U3 += 1;
      }
    if(ps2x.ButtonPressed(PSB_PAD_DOWN))  {          //will be TRUE if button was JUST pressed
        m1 += 0.1;
      }
    if(ps2x.ButtonPressed(PSB_PAD_UP))  {          //will be TRUE if button was JUST pressed
        n1 += 0.1;
      }
    if(ps2x.ButtonPressed(PSB_PAD_LEFT))  {          //will be TRUE if button was JUST pressed
        m2 += 0.1;
      }
    if(ps2x.ButtonPressed(PSB_PAD_RIGHT))  {          //will be TRUE if button was JUST pressed
        n2 += 0.1;
      }
    float strecke1 = m1*x1;
    float strecke2 = m2*x2;
    //Start:
    if(ps2x.ButtonPressed(PSB_START))  {          //will be TRUE if button was JUST pressed
        speedy = 100;
        distance = 0.0;
        while (distance < strecke1/100)  {
          Motor("vr",1); 
          Motor("vl",1);  
          Motor("hr",1);  
          Motor("hl",1); 
          attachInterrupt(digitalPinToInterrupt(2), blink, RISING);
        }
        Motor("vr",0); 
        Motor("vl",0);  
        Motor("hr",0);  
        Motor("hl",0);
        if (U1 == 1) {DrehungGegenUhr(n1,a1);}
        if (U1 > 1) {DrehungInUhr(n1,a1);}
        distance = 0.0;
        while (distance < strecke2/100)  {
          Motor("vr",1); 
          Motor("vl",1);  
          Motor("hr",1);  
          Motor("hl",1); 
          attachInterrupt(digitalPinToInterrupt(2), blink, RISING);
        }
        Motor("vr",0); 
        Motor("vl",0);  
        Motor("hr",0);  
        Motor("hl",0);
        if (U2 == 1) {DrehungGegenUhr(n2,a2);}
        if (U2 > 1) {DrehungInUhr(n2,a2);}
        distance = 0.0;
        while (distance < strecke1/100)  {
          Motor("vr",1); 
          Motor("vl",1);  
          Motor("hr",1);  
          Motor("hl",1); 
          attachInterrupt(digitalPinToInterrupt(2), blink, RISING);
        }
        Motor("vr",0); 
        Motor("vl",0);  
        Motor("hr",0);  
        Motor("hl",0);
        if (U3 == 1) {DrehungGegenUhr(n1,a1);}
        if (U3 > 1) {DrehungInUhr(n1,a1);}
        distance = 0.0;
        while (distance < strecke2/100)  {
          Motor("vr",1); 
          Motor("vl",1);  
          Motor("hr",1);  
          Motor("hl",1); 
          attachInterrupt(digitalPinToInterrupt(2), blink, RISING);
        }
        Motor("vr",0); 
        Motor("vl",0);  
        Motor("hr",0);  
        Motor("hl",0);
        activateStation5 = 0;
        clearVaribles();
    }
    return true;
  }

  bool Motorboth(String side,int direction = 0,int speed = 255) {
    if (speed == 252){speed = 255;}
      if (side == "r") {
      Motor("hr",direction,speed);
      Motor("vr",direction,speed);}
    else if (side == "l") {
      Motor("hl",direction,speed);
      Motor("vl",direction,speed);}
  }

  void clearVaribles() {
    distance = 0.0;
    activateStation5 = 0; //0=false, 1=true
    m1 = 0.0;
    n1 = 0.0;
    m2 = 0.0;
    n2 = 0.0;
    U1 = 0;
    U2 = 0;
    U3 = 0;
  }

//Ultraschallsensor====================================================================================================================================================================================
  void Input_Ultraschallsensor() {
    digitalWrite(Ultraschall_TriggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(Ultraschall_TriggerPin, LOW);
    unsigned long duration = pulseIn(Ultraschall_EchoPin, HIGH);
    Ultraschall_Distance = duration * 0.03432 / 2;
  }

//Servo================================================================================================================================================================================================
  void Output_ServoCtrl() {
    //Definitionen=================================================================================
      int Servo_PulsInkrement = 10;

    //Istwertbestimmung============================================================================
      if (Servo_Langsam) {
        if (Servo_PulsIst < Servo_PulsSoll) {
          Servo_PulsIst = Servo_PulsIst + Servo_PulsInkrement;
        }
        else if (Servo_PulsIst < Servo_PulsSoll) {
          Servo_PulsIst = Servo_PulsIst - Servo_PulsInkrement;
        }
      }
      else{
        Servo_PulsIst = Servo_PulsSoll;
      }

    //Pulsübergabe=================================================================================
      pwm.setPWM(0, 0, Servo_PulsIst);
  }

void Motor1() {
  ps2x.read_gamepad(false, vibrate);          //read controller and set large motor to spin at 'vibrate' speed
    
    if(ps2x.Button(PSB_START)){}                   //will be TRUE as long as button is pressed
    //Serial.println("Start is being held");
    if(ps2x.Button(PSB_SELECT)) {
      //Serial.println("Select is being held");
      clearVaribles();
    }
    if(ps2x.Button(PSB_R2)) {
      Motor("vr",1); 
      Motor("vl",1);  
      Motor("hr",1);  
      Motor("hl",1); 
    }
      if(ps2x.Button(PSB_L2)) {
      Motor("vr",2); 
      Motor("vl",2);  
      Motor("hr",2);  
      Motor("hl",2); 
    }
    vibrate = ps2x.Analog(PSAB_BLUE);        //this will set the large motor vibrate speed based on 
                                              //how hard you press the blue (X) button    
    if (ps2x.NewButtonState()) {              //will be TRUE if any button changes state (on to off, or off to on)
        if(ps2x.Button(PSB_L1)) {
        // Serial.println("L1 pressed");
         Motor("vr",1); 
         Motor("vl",0);  
         Motor("hr",1);  
         Motor("hl",0); 
         delay(10);
        }
        if(ps2x.Button(PSB_R1)) {
        // Serial.println("R1 pressed");
         Motor("vr",0); 
         Motor("vl",1);  
         Motor("hr",0);  
         Motor("hl",1); 
         delay(10);
        }
        if(ps2x.Button(PSB_L2)) {
        // Serial.println("L2 pressed");
        }
        if(ps2x.Button(PSB_R2)) {
         //Serial.println("R2 pressed");
        }
        if(ps2x.Button(PSB_GREEN)){}
         //Serial.println("Triangle pressed");
    }   
    if(ps2x.ButtonPressed(PSB_RED))  {          //will be TRUE if button was JUST pressed
      //Serial.println("Circle just pressed");
      if (speedy <= 245) speedy += 10;
    }
    if(ps2x.ButtonPressed(PSB_PINK))  {      
     // Serial.println("Square just pressed");
      if (speedy >= 10) speedy -= 10;
    }    
    if(ps2x.ButtonReleased(PSB_PINK)) {}            //will be TRUE if button was JUST released
       //  Serial.println("Square just released"); 
    if(ps2x.ButtonReleased(PSB_R2)) {
      Motor("vr",0); 
      Motor("vl",0);  
      Motor("hr",0);  
      Motor("hl",0); 
      //Serial.println("R2 just released"); 
    }    
    if(ps2x.ButtonReleased(PSB_L2)) {
      Motor("vr",0); 
      Motor("vl",0);  
      Motor("hr",0);  
      Motor("hl",0); 
      //Serial.println("L2 just released"); 
    }
    if(ps2x.ButtonReleased(PSB_R1)) {
      Motor("vr",0); 
      Motor("vl",0);  
      Motor("hr",0);  
      Motor("hl",0); 
      //Serial.println("R1 just released"); 
    }
    if(ps2x.ButtonReleased(PSB_L1)) {
      Motor("vr",0); 
      Motor("vl",0);  
      Motor("hr",0);  
      Motor("hl",0); 
      //Serial.println("L1 just released"); 
    }
    if(ps2x.NewButtonState(PSB_BLUE)) {           //will be TRUE if button was JUST pressed OR released
        // Serial.println("X just changed");
    }
}

void Motor2() {
  int speedy = 0;

  //DualShock Controller
  ps2x.read_gamepad(false, vibrate);  //read controller and set large motor to spin at 'vibrate' speed

  if (ps2x.Button(PSB_START)) {} //will be TRUE as long as button is pressed
    //Serial.println("Start is being held");
  if (ps2x.Button(PSB_SELECT)){}
    //Serial.println("Select is being held");

  if (ps2x.Button(PSB_PAD_UP)) {  //will be TRUE as long as button is pressed
    //Serial.print("Up held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
  }
  if (ps2x.Button(PSB_PAD_RIGHT)) {
    //Serial.print("Right held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
  }
  if (ps2x.Button(PSB_PAD_LEFT)) {
    //Serial.print("LEFT held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
  }
  if (ps2x.Button(PSB_PAD_DOWN)) {
    //Serial.print("DOWN held this hard: ");
    Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
  }
  vibrate = ps2x.Analog(PSAB_CROSS);  //this will set the large motor vibrate speed based on how hard you press the blue (X) button
  if (ps2x.NewButtonState()) {        //will be TRUE if any button changes state (on to off, or off to on)
    if (ps2x.Button(PSB_L3)){}
      //Serial.println("L3 pressed");
    if (ps2x.Button(PSB_R3)){}
      //Serial.println("R3 pressed");
    if (ps2x.Button(PSB_L2)) {
    //raum für ideen
    }
    if (ps2x.Button(PSB_R2)) {
    //raum für ideen
    }
    if (ps2x.Button(PSB_TRIANGLE)){
      //Serial.println("Triangle pressed");
    }
    if (ps2x.NewButtonState(PSB_CROSS)) {} //will be TRUE if button was JUST pressed OR released
      //Serial.println("X just changed");
    if (ps2x.ButtonReleased(PSB_SQUARE)) {} //will be TRUE if button was JUST released
      //Serial.println("Square just released");
  }
    

    int direction_l = 0;
    int direction_r = 0;
    int y = ps2x.Analog(PSS_RY);
    int x = ps2x.Analog(PSS_LX);
    int speedy_r = 0;
    int speedy_l = 0;
    if (y == 127 && x == 128) 
    {
      Motorboth("l"); 
      Motorboth("r");
    }
    if (y < 127)
    {
      
      speedy = (255/126)*(126-y);
      speedy_l = speedy;
      speedy_r = speedy; 
      direction_l = 1;
      direction_r = 1;      

    }

        if (y > 127)
    {
      
      speedy = (255/127)*(y-129);
      speedy_l = speedy;
      speedy_r = speedy;
      direction_l = 2;
      direction_r = 2;
    }

    
    if (x < 128&& y == 127)
    { 
      speedy = (255/126)*(126-x);
      speedy_l = speedy;
      speedy_r = speedy; 
      //speedy_l =(255/127)*(127-x);
      //Serial.println(speedy_l);
      direction_l = 2;
      direction_r = 1;  
    }



        if (x > 128 && y == 127)
    {
      speedy = (255/127)*(x-129);
      speedy_l = speedy;
      speedy_r = speedy;
      //speedy_l =(255/127)*(127-x);
      //Serial.println(speedy_l);
      direction_l = 1;
      direction_r = 2;  


    }

        if (x < 128&& y != 127)
    { 
      //links rumm
      speedy = (255/126)*(126-x);
      speedy_l = speedy_r- speedy;
      //speedy_l =(255/127)*(127-x);
      //Serial.println(speedy_l);
      
    }



        if (x > 128 && y != 127)
    {
      speedy = (255/127)*(x-129);
      speedy_r = speedy_l-speedy;
      //speedy_l =(255/127)*(127-x);
      //Serial.println(speedy_l);
       


    }
  
      Motorboth("l",direction_l,speedy_l);
      Motorboth("r",direction_r,speedy_r);
      digitalWrite(A11,255);
}
void Stationswahl() {
      switch (StationsNrAktuell) {
      case 0: //keine Station gewählt => manuelles Fahren

      case 1: //Station 1 Aktiv

      case 2: //Station 2 Aktiv

      case 3: //Station 3 Aktiv

      case 4: //Station 4 Aktiv

    }
  }
//Loop=================================================================================================================================================================================================
  void loop() {

    //Interne Definitionen=========================================================================
      uint8_t ZeitDelta_Ultraschallsensor = 50;
      uint8_t ZeitDelta_Servo = 50;
      uint8_t ZeitDelta_AblaufStation14 = 500;
      uint8_t ZeitDelta_Station14BoxenRegistrieren = 50;
      

    //Zeit=========================================================================================
      Zeit_Laufzeit = millis();

    //Input========================================================================================
      if (Station14_Messung && Zeit_Laufzeit - Zeit_Ultraschallsensor >= ZeitDelta_Ultraschallsensor) {
        Zeit_Ultraschallsensor = Zeit_Laufzeit;
        Input_Ultraschallsensor();
      }

    //Ablaufsteuerung==============================================================================
      if (Zeit_Laufzeit - Zeit_AblaufStation14 >= ZeitDelta_AblaufStation14) {
        Zeit_AblaufStation14 = Zeit_Laufzeit;
        //Ablauf_Station_1_4();
      }
      if (Station14_RegistriereBoxen && Zeit_Laufzeit - Zeit_Station14BoxenRegistrieren > ZeitDelta_Station14BoxenRegistrieren) {
        Zeit_Station14BoxenRegistrieren = Zeit_Laufzeit;
        //Station_1_4_BoxenRegistrieren();
      }

    //Output=======================================================================================
      if (Zeit_Laufzeit - Zeit_Servo >= ZeitDelta_Servo) {
        ZeitDelta_Servo = Zeit_Laufzeit;
        Output_ServoCtrl();
      }
      
    //DualShock Controller
    Motor1();
    Motor2();
  //Aktivieren Station 5
    if(ps2x.ButtonPressed(PSB_GREEN))  {      
      if (activateStation5 == 0) {activateStation5 = 1;
        return;
      }
      }
    if (activateStation5 == 1) {
      Station5(Station5_Abstand1, Station5_Abstand2, Station5_Winkel1, Station5_Winkel2);
    }

}
