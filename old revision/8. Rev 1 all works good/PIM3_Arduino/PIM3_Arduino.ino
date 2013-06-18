
#include <EEPROM.h>
#include "EEPROMAnything.h"
//om the first byte (address 0) of the EEPROM
int address = 0;   // EPROM Adress
//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt(int p_address, int p_value)
      {
      byte lowByte = ((p_value >> 0) & 0xFF);
      byte highByte = ((p_value >> 8) & 0xFF);

      EEPROM.write(p_address, lowByte);
      EEPROM.write(p_address + 1, highByte);
      }

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
unsigned int EEPROMReadInt(int p_address)
      {
      byte lowByte = EEPROM.read(p_address);
      byte highByte = EEPROM.read(p_address + 1);

      return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
      }
      
      //------------HOW TO USE IT?------------see below-----
      //EEPROM_readAnything(0, configuration);
      //EEPROM_writeAnything(0, configuration);

//-----------------------------------------------------------------------------------------------

// --------------motor A I/O for Lift---------
const int DIR_A=22;
const int PWM_A=9;     //----------------------------------PWM output to motor A--------------
//----------------motor B I/O --------
const int DIR_B=23;
const int PWM_B=10;   // -----------------------------------PWM output to motor B------------

// ---------------IO power and start-point switch---
const int Set_status=4;                               // used to write and read the hall hall_A1_counter value in the EEPROM
const int start_point_switch=7;                      // as start point for motion. it used to calibare to zero position the lift actuators.
const int switch_for_hand_grip=5;                    // it is not used for now. 
const int up_button=12;                             // Up button
const int down_button=11;                           // down button
const int cal_button=8;                             // calibrate the hand guide to start position

//---------------motor C I/O for hand grip-----------
const int DIR_C=50;
const int PWM_C=6;
const int Max_speed_h=200; // maximum speed of hand grip          //----------------------------------------------Speed hand grip
const int i=200;
const int Max_speed_lift=255; // maximum speed for lift and down //-----------------------------------------------Speed lift or lower
//---------------LED output-----------------------------
const int LED_low=24;
const int LED_midle=26;
const int LED_high=28;
//-----------------
const int hallPinA_1 = 2;     // A_the number of the hall effect sensor pin
const int hallPinA_2 = 3;    //// A_the number of the hall effect sensor pin, it is digital input only referens direction
// variables will change:
int hall_A1_counter;          // variable for reading the hall sensor status
int hall_A2_counter; 
int PIN=13;
int status=LOW;
int hall_counter=0;
int hall_real=0;
int forward, backward, calibrate;
const int start_position=110; ///-----------------------------------------------------Start Position//////////////////////////////////////////////////////////////////////////////////
int duty_a_backward=0;
int duty_a_forward=0;
int duty_c_h_forward=0;
int duty_c_h_backward=0;
int duty_b_backward=0;
int duty_b_forward=0;
char My_position[] = "arduino";

enum my_position {
  MY_POSITION_T0,
  MY_POSITION_T1,
  MY_POSITION_T2,
};

enum my_position my_current_position;

char stop_posion[] = "arduino";

enum stop_position {
  stop_position_low,
  stop_position_midle,
  stop_position_high,
};

enum stop_position my_stop_position;

/////////////////////////////////////

int stop_flag_h=0;
int stop_flag_L=0;
//----------------------------------Sensor_with potentiometer 20K----

int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
int stop_position=100;//---------------------------------------------------------------Stop position----------------------------------------------------
int button_check=0;
int set_status=0;

void setup()
{
  
   
  Serial.begin(57600);
  pinMode(up_button, INPUT); 
  pinMode(down_button, INPUT);
  pinMode(cal_button, INPUT); 
  pinMode(PIN, OUTPUT);
  pinMode(DIR_C,OUTPUT);
  pinMode(DIR_A,OUTPUT);
  pinMode(DIR_B,OUTPUT);
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(PWM_C, OUTPUT);
  pinMode(start_point_switch,INPUT);
  pinMode(switch_for_hand_grip,INPUT);
  pinMode(Set_status,INPUT);
  pinMode(LED_low, OUTPUT);
  pinMode(LED_midle,OUTPUT);
  pinMode(LED_high,OUTPUT);
  //value= EEPROM.read(address);
   hall_A1_counter=EEPROMReadInt(0);
        // leave other bits as set by arduino init() in wiring.c
//        byte mask = B11111001;
//        TCCR4B &= mask; // TCCR4B is now xxxxx000
//        //
//        // set CS22:20 in TCCR4B see p 156 of datasheet
//        TCCR4B |= (0<<CS22) | (0<<CS21) | (1<<CS20); // same as TCCR4B |= B00000001; TCCR4B is now xxxxx001
////  
////  
        TCCR2B = TCCR2B & 0b11111000 | 0x01;
        TCCR4B = TCCR4B & 0b11111000 | 0x01;
        int i=200;
}


// main loop begins here

void loop()
{
  
  Serial.println(hall_A1_counter);
  sensorValue = analogRead(sensorPin);      // Read value from the potentiometer 
  attachInterrupt(0, hall_A, RISING);
  calibrate=digitalRead(cal_button);        //Reading the cal-button
  forward=digitalRead(up_button);           //Reading the forward button 
  backward=digitalRead(down_button);        //Reading the backward button
  float handguide_position = sensorValue/4;  // maps 0-1024 to 0-256   value from potentiometer
  int dada=digitalRead(switch_for_hand_grip);
  
  
  if (digitalRead(Set_status)==HIGH)  {
    set_status++;
    delay(300);
  }
  
  if(set_status=0) { my_stop_position==stop_position_low; digitalWrite(LED_low,HIGH); }
  if(set_status=1) { my_stop_position==stop_position_midle; digitalWrite(LED_midle,HIGH);}
  if(set_status=2) { my_stop_position==stop_position_high; digitalWrite(LED_high,HIGH);} 
//  if (digitalRead(Set_status)==HIGH)  {
//    EEPROMWriteInt(0, hall_A1_counter);
//    delay(300);
//  }
  
    //--------------------------------calibration Routin Start here------------------------------------------------------
                                               if(calibrate==HIGH) 
                                               {  button_check=1;
                                                 delay(100);
                                                 if(calibrate==LOW) button_check=0;
                                                 if(calibrate==HIGH && button_check==1)
                                                 {
                                                         if (handguide_position<start_position)
                                                         {
                                                          
                                                           digitalWrite(DIR_C, LOW);
                                                           analogWrite(PWM_C, 80);
                                                                       }
                                                         if (handguide_position>=start_position)
                                                         {
                                                           
                                                           digitalWrite(DIR_C, HIGH);
                                                           analogWrite(PWM_C, 80);
                                                                     }
                                                                     
                                                         stop_flag_h=1;
                                                         
                                                         stop_flag_L=1;
                                            
                                                         }
                                               }
                                               int sarang_start_position;
                                             sarang_start_position==start_position%5;
                                     //Serial.println(start_position);        
                                          if (handguide_position==start_position&& stop_flag_h==1)
                                               {                         
                                                 analogWrite(PWM_C, 0);
                                                 stop_flag_h=0;
                                                 button_check=0;
                                               }
                                               
                                               
                                               if( stop_flag_L==1)
                                               {     digitalWrite(DIR_A, LOW);
                                                     analogWrite(PWM_A, 200);
                                                     digitalWrite(DIR_B, LOW);
                                                     analogWrite(PWM_B, 200);
                                                 if (digitalRead(start_point_switch)==HIGH)  {
                                                         button_check=1;
                                                        analogWrite(PWM_A, 0);
                                                        analogWrite(PWM_B, 0);
                                                        //delay(100);                                                       
                                                       if (digitalRead(start_point_switch)==LOW) button_check=0;
                                                       if (digitalRead(start_point_switch)==HIGH && button_check==1) //hall_A1_counter=0;
                                                       stop_flag_L=0;
                                                       button_check=0;
                                                     }
                                                        
                                                                  }
                                                                        
                                                if (digitalRead(start_point_switch)==HIGH) 
                                                {    button_check=1; 
                                                     //delay(100);
                                                     if (digitalRead(start_point_switch)==LOW) button_check=0;
                                                     if (digitalRead(start_point_switch)==HIGH && button_check==1) hall_A1_counter=0;
                                                       
                                                               }
                                              
                                                 
                //*******************************************************Lift start here ***********************************************************
                
                if (forward==HIGH)
               { 
              
                 switch (my_current_position) {
                    case MY_POSITION_T0:  
                      Serial.println("T0");
                      if (handguide_position>=20 && handguide_position<=160)
                          {
                                     
                                 if (handguide_position>start_position-10 && handguide_position<=start_position+10) forward_C();// Handgrip is　going  
                                 if (handguide_position<start_position-5)
                                    {
                                      forward_A(); //// lift　is　going 
                                      forward_B(); // lift　is　going 
                                    }
                                 if (handguide_position<=start_position-10) handgrip_stop_forward(); // // handgrip stops
                                               
                                   }
                      break;
                    case MY_POSITION_T1:    // your hand is close to the sensor
                      Serial.println("   T1");
                        
                                      forward_A(); // lift　is　going 
                                      forward_B(); // lift　is　going 
                                      if (handguide_position<=start_position-10) backward_C();
                                     if (handguide_position>start_position+5)
                      {
                        
                       //analogWrite(PWM_C, 0);
                      handgrip_stop_backward();
                      handgrip_stop_forward();
                       
                        
                      } 
                      break;
                    case MY_POSITION_T2:    // 
                      Serial.println("         T2");
                      if (handguide_position<start_position) backward_C(); 
                      if (hall_A1_counter>stop_position) stop();
                    
                      if (handguide_position>start_position+5)
                      {
                        
                       //analogWrite(PWM_C, 0);
                      handgrip_stop_backward();
                      handgrip_stop_forward();
                       
                        
                      }
                      break;
                    default:    // 
                      Serial.println("                       T3");
                      break;
                                              }  // switch case ends here
               }                            // up ends here
// --------------------------------------backward--------------------------------------starts here            

             if (backward==HIGH)
              {
                if (digitalRead(start_point_switch)==HIGH)  {
                      analogWrite(PWM_A, 0);
                      analogWrite(PWM_B, 0);
                      delay(500);
                      if (digitalRead(start_point_switch)==HIGH) hall_A1_counter=0;
                      duty_a_backward=0;
                      duty_b_backward=0;
                    }
                    switch (my_current_position) {
                    case MY_POSITION_T0:    // your hand is on the sensor
                      Serial.println("T0");    
                                 digitalWrite(DIR_C, HIGH);   
                                 if ( handguide_position>=start_position) analogWrite(PWM_C, 90);// Handgrip is　going  
                                 if (hall_A1_counter<=14) stop_end();
                                 if (handguide_position>start_position-10 && handguide_position<=start_position+10) analogWrite(PWM_C, 0);          
                      break;
                    case MY_POSITION_T1:    // 
                      Serial.println("   T1");
                        my_current_position=MY_POSITION_T1;
                        digitalWrite(DIR_C, HIGH);   
                                 if ( handguide_position>=start_position) analogWrite(PWM_C, 90);// Handgrip is　going  
                                 backward_A();
                                 backward_B();
                                 if (handguide_position>start_position-10 && handguide_position<=start_position+10) analogWrite(PWM_C, duty_a_forward);                           
                      break;
                    case MY_POSITION_T2:    // 
                      Serial.println("         T2");
                     // if (handguide_position>start_position+20 && handguide_position<150) forward_C();  // going reverse stops at 80
//                      if (handguide_position<=125) 
//                       { 
                         backward_A();
                         backward_B();
//                       }
//                      if (handguide_position<=130) 
//                      {
//                        //stop();
//                        handgrip_stop_forward();
//                       
//                      }
                      break;
                    default:    // 
                      Serial.println("                       T3");
                      break;
                                              }
     
                if (hall_A1_counter<=200) 
                {
                  digitalWrite(PIN, LOW);
                       } 
                            }
          if (backward==LOW && forward==LOW)    // if the buttons are released 
          {
            stop();
            handgrip_stop_forward();
            handgrip_stop_backward();
            int i=200;
          }

          if (hall_A1_counter>=-50&& hall_A1_counter<=30) my_current_position = MY_POSITION_T0;
          if (hall_A1_counter>30&& hall_A1_counter<=stop_position) my_current_position = MY_POSITION_T1;
          if (hall_A1_counter>stop_position&& hall_A1_counter<=150) my_current_position = MY_POSITION_T2;
          
          switch (my_stop_position) {
            case stop_position_low:
                  stop_position=100;
                  if (hall_A1_counter>=-50&& hall_A1_counter<=30) my_current_position = MY_POSITION_T0;
                  if (hall_A1_counter>30&& hall_A1_counter<=stop_position) my_current_position = MY_POSITION_T1;
                  if (hall_A1_counter>stop_position&& hall_A1_counter<=150) my_current_position = MY_POSITION_T2;
             break;
             case stop_position_midle:
                  if (hall_A1_counter>=-50&& hall_A1_counter<=30) my_current_position = MY_POSITION_T0;
                  if (hall_A1_counter>30&& hall_A1_counter<=stop_position) my_current_position = MY_POSITION_T1;
                  if (hall_A1_counter>stop_position&& hall_A1_counter<=150) my_current_position = MY_POSITION_T2;
                 stop_position=110;
             break;
             case stop_position_high:
                 stop_position=120;
                 if (hall_A1_counter>=-50&& hall_A1_counter<=30) my_current_position = MY_POSITION_T0;
                 if (hall_A1_counter>30&& hall_A1_counter<=stop_position) my_current_position = MY_POSITION_T1;
                 if (hall_A1_counter>stop_position&& hall_A1_counter<=150) my_current_position = MY_POSITION_T2;
             break;
            
            default:    // 
                      
                      break;
                                              }
          
 //delay(20);              
//Serial.println(hall_A1_counter);       
Serial.println(handguide_position);
// Serial.println(duty_a_backward);
}

//----------------------------------------------Functions listed below------------------------------------------------------
void hall_A()             
{
  
  if (digitalRead(hallPinA_2)==LOW)
  {
   hall_A1_counter--;
}
else hall_A1_counter++;
}
//
//--------------function Motor A forward-------------------------------------------------------------------
void forward_A()
{
  digitalWrite(DIR_A, HIGH);
  if (duty_a_forward<Max_speed_lift) 
  {
  duty_a_forward++;
  analogWrite(PWM_A, duty_a_forward);
  delay(2);
  }
}

//Motor A backward forward
void backward_A()
{
  digitalWrite(DIR_A, LOW);
  if (duty_a_backward<Max_speed_lift)
 { 
   duty_a_backward++;
  analogWrite(PWM_A, duty_a_backward);
  delay(2);
 }
}

//------------------function Motor B forward--------------------------------------------------------
void forward_B()
{
  digitalWrite(DIR_B, HIGH);
  if (duty_b_forward<Max_speed_lift) 
  {
  duty_b_forward++;
  analogWrite(PWM_B, duty_b_forward);
  delay(2);
  }
}

//------------------function Motor B backward ------
void backward_B()
{
  digitalWrite(DIR_B, LOW);
  if (duty_b_backward<Max_speed_lift) 
  {
  duty_b_backward++;
  analogWrite(PWM_B, duty_b_backward);
  delay(2);
  }
}

//------------------function Motor C forward----------------------------------
void forward_C()
{
  digitalWrite(DIR_C, HIGH);
  if (duty_c_h_forward<Max_speed_h)
   duty_c_h_forward++;
else
 duty_c_h_forward = Max_speed_h;
 
  analogWrite(PWM_C, duty_c_h_forward);
  delay(2);
}

//------------------function Motor C backward ------
void backward_C()
{
  digitalWrite(DIR_C, LOW);

  if (duty_c_h_backward<Max_speed_h)
   duty_c_h_backward++;
  else
  
  duty_c_h_backward = Max_speed_h;
  
  analogWrite(PWM_C, duty_c_h_backward);
  delay(2);
}

//------------------Stop A&B Motor ------

void stop()
{
 
  if (duty_a_forward >= 1) {
    analogWrite(PWM_A, duty_a_forward--);
    analogWrite(PWM_B, duty_b_forward--);
    delay(1);
  }
 
  if (duty_a_backward >= 1) {
    analogWrite(PWM_A, duty_a_backward--);
    analogWrite(PWM_B, duty_b_backward--);
    delay(1);
  }
}


void handgrip_stop_forward()
{
 
  if (duty_c_h_forward >= 1) {
    analogWrite(PWM_C, duty_c_h_forward--);
    delay(1);
  }
}
 void handgrip_stop_backward()
 {
 if (duty_c_h_backward >= 1) {
    analogWrite(PWM_C, duty_c_h_backward--);
    //delay(1);
  }
}

void stop_end()
{
 
  if (duty_a_forward >= 1) {
    analogWrite(PWM_A, duty_a_forward--);
    analogWrite(PWM_B, duty_b_forward--);
    delay(8);
  }
 
  if (duty_a_backward >= 1) {
    analogWrite(PWM_A, duty_a_backward--);
    analogWrite(PWM_B, duty_b_backward--);
    delay(8);
  }
}
