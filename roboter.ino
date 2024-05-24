#define Gyro_SCL 5
#define Gyro_XDA 4
#define Ultra_Trig 46
#define Ultra_Echo 48
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

#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *Motor_1 = AFMS.getMotor(1);
Adafruit_DCMotor *Motor_2 = AFMS.getMotor(2);
Adafruit_DCMotor *Motor_3 = AFMS.getMotor(3);
Adafruit_DCMotor *Motor_4 = AFMS.getMotor(4);






#include <PS2X_lib.h>  //for v1.6

/******************************************************************
 * set pins connected to PS2 controller:
 *   - 1e column: original 
 *   - 2e colmun: Stef?
 * replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT 22  //14
#define PS2_CMD 23  //15
#define PS2_SEL 24  //16
#define PS2_CLK 25  //17

/******************************************************************
 * select modes of PS2 controller:
 *   - pressures = analog reading of push-butttons 
 *   - rumble    = motor rumbling
 * uncomment 1 of the lines for each mode selection
 ******************************************************************/
//#define pressures   true
#define pressures false
//#define rumble      true
#define rumble false

PS2X ps2x;  // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning
//you must always either restart your Arduino after you connect the controller,
//or call config_gamepad(pins) again after connecting the controller.

int error = 0;
byte type = 0;
byte vibrate = 0;









void setup() {
  Serial.begin(9600);  // set up Serial library at 9600 bps
  Serial.println("Adafruit Motorshield v2 - DC Motor test!");

  if (!AFMS.begin()) {  // create with the default frequency 1.6KHz
                        // if (!AFMS.begin(1000)) {  // OR with a different frequency, say 1KHz
    Serial.println("Could not find Motor Shield. Check wiring.");
    while (1)
      ;
  }
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














  delay(3000);  //added delay to give wireless ps2 module some time to startup, before configuring it

  //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************

  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);

  if (error == 0) {
    Serial.print("Found Controller, configured successful ");
    Serial.print("pressures = ");
    if (pressures)
      Serial.println("true ");
    else
      Serial.println("false");
    Serial.print("rumble = ");
    if (rumble)
      Serial.println("true)");
    else
      Serial.println("false");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Note: Go to www.billporter.info for updates and to report bugs.");
  } else if (error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

  else if (error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if (error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

  //  Serial.print(ps2x.Analog(1), HEX);

  type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.print("Unknown Controller type found ");
      break;
    case 1:
      Serial.print("DualShock Controller found ");
      break;
    case 2:
      Serial.print("GuitarHero Controller found ");
      break;
    case 3:
      Serial.print("Wireless Sony DualShock Controller found ");
      break;
  }
}




bool Motor(String Motor, int direction = 0, int speed = 100) {

  if (Motor == "hr") {
    Motor_1->setSpeed(speed);
    if (direction == 0) {
      Motor_1->run(RELEASE);
    } else if (direction == 1) {
      Motor_1->run(FORWARD);
    } else if (direction == 2) {
      Motor_1->run(BACKWARD);
    }
    return true;
  }


  else if (Motor == "hl") {
    Motor_4->setSpeed(speed);
    if (direction == 0) {
      Motor_4->run(RELEASE);
    } else if (direction == 1) {
      Motor_4->run(FORWARD);
    } else if (direction == 2) {
      Motor_4->run(BACKWARD);
    }
    return true;
  }

  else if (Motor == "vr") {
    Motor_2->setSpeed(speed);
    if (direction == 0) {
      Motor_2->run(RELEASE);
    } else if (direction == 1) {
      Motor_2->run(FORWARD);
    } else if (direction == 2) {
      Motor_2->run(BACKWARD);
    }


    return true;
  }



  else if (Motor == "vl") {
    Motor_3->setSpeed(speed);
    if (direction == 0) {
      Motor_3->run(RELEASE);
    } else if (direction == 1) {
      Motor_3->run(FORWARD);
    } else if (direction == 2) {
      Motor_3->run(BACKWARD);
    }
    return true;
  }
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


bool drive_along_line()
  { 
    delay(100);
    while (ps2x.ButtonPressed(PSB_CIRCLE) != 1){delay(1);}
    while (1){
    int rechts = analogRead(A1);
    int links = analogRead(A0);
    Serial.print(links);
    Serial.print("----");
    Serial.println(rechts);
    
    //if (links < 40 && rechts < 40){return true;}
    if (links < 60 && rechts < 60)
    {
      motorboth("r",1)
    }

  }
  }



void loop() {

  int speedy = 0;



  /* You must Read Gamepad to get new values and set vibration values
     ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
     if you don't enable the rumble, use ps2x.read_gamepad(); with no values
     You should call this at least once a second
   */
  if (error == 1)  //skip loop if no controller found
    return;

  if (type == 2) {        //Guitar Hero Controller
    ps2x.read_gamepad();  //read controller

    if (ps2x.ButtonPressed(GREEN_FRET))
      Serial.println("Green Fret Pressed");
    if (ps2x.ButtonPressed(RED_FRET))
      Serial.println("Red Fret Pressed");
    if (ps2x.ButtonPressed(YELLOW_FRET))
      Serial.println("Yellow Fret Pressed");
    if (ps2x.ButtonPressed(BLUE_FRET))
      Serial.println("Blue Fret Pressed");
    if (ps2x.ButtonPressed(ORANGE_FRET))
      Serial.println("Orange Fret Pressed");

    if (ps2x.ButtonPressed(STAR_POWER))
      Serial.println("Star Power Command");

    if (ps2x.Button(UP_STRUM))  //will be TRUE as long as button is pressed
      Serial.println("Up Strum");
    if (ps2x.Button(DOWN_STRUM))
      Serial.println("DOWN Strum");

    if (ps2x.Button(PSB_START))  //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
    if (ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");

    if (ps2x.Button(ORANGE_FRET)) {  // print stick value IF TRUE
      Serial.print("Wammy Bar Position:");
      Serial.println(ps2x.Analog(WHAMMY_BAR), DEC);
    }
  } else {                              //DualShock Controller
    ps2x.read_gamepad(false, vibrate);  //read controller and set large motor to spin at 'vibrate' speed

    if (ps2x.Button(PSB_START))  //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
    if (ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");

    if (ps2x.Button(PSB_PAD_UP)) {  //will be TRUE as long as button is pressed
      Serial.print("Up held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
    }
    if (ps2x.Button(PSB_PAD_RIGHT)) {
      Serial.print("Right held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
    }
    if (ps2x.Button(PSB_PAD_LEFT)) {
      Serial.print("LEFT held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
    }
    if (ps2x.Button(PSB_PAD_DOWN)) {
      Serial.print("DOWN held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
    }

    vibrate = ps2x.Analog(PSAB_CROSS);  //this will set the large motor vibrate speed based on how hard you press the blue (X) button
    if (ps2x.NewButtonState()) {        //will be TRUE if any button changes state (on to off, or off to on)
      if (ps2x.Button(PSB_L3))
        Serial.println("L3 pressed");
      if (ps2x.Button(PSB_R3))
        Serial.println("R3 pressed");
      if (ps2x.Button(PSB_L2)) {
      //raum für ideen
      }
      if (ps2x.Button(PSB_R2)) {
      //raum für ideen
      }
      if (ps2x.Button(PSB_TRIANGLE))
        Serial.println("Triangle pressed");
    }

    if (ps2x.ButtonPressed(PSB_CIRCLE)) {
    drive_along_line();    
    }  //will be TRUE if button was JUST pressed

    if (ps2x.NewButtonState(PSB_CROSS))  //will be TRUE if button was JUST pressed OR released
      Serial.println("X just changed");
    if (ps2x.ButtonReleased(PSB_SQUARE))  //will be TRUE if button was JUST released
      Serial.println("Square just released");

    if (ps2x.Button(PSB_L1)) {
    //raum für ideen
    }
    if (ps2x.Button(PSB_R1)) {
    //raum für ideen :)
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
  delay(50);
}
