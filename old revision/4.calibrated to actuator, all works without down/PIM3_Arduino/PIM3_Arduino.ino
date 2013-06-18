
#include <EEPROM.h>
#include "EEPROMAnything.h"
// Always get the EEPROM in the same order
// This will ensure that each variable gets
// the same adress after every restart

//int addressFloat = EEPROM.getAddress(sizeof(float));
// start reading from the first byte (address 0) of the EEPROM
int address = 0;



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
      
//-----------------------------------------------------------------------------------------------

//EEPROM_readAnything(0, configuration);
//EEPROM_writeAnything(0, configuration);

// --------------motor A I/O for Lift---------
const int DIR_A=22;
const int PWM_A=9;
//----------------motor B I/O --------
const int DIR_B=23;
const int PWM_B=8;

// ---------------IO power and start-point switch---
const int power_off=4;
const int start_point_switch=7;
const int switch_for_hand_grip=5;
//---------------motor C I/O for hand grip-----------
const int DIR_C=50;
const int PWM_C=6;
const int Max_speed_h=50; // maximum speed of hand grip
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
const int start_position=120;
int duty_c_backward=0;
int duty_c_forward=0;
int duty_c_h_forward=0;
int duty_c_h_backward=0;
char My_position[] = "arduino";
enum my_position {
  MY_POSITION_T0,
  MY_POSITION_T1,
  MY_POSITION_T2,
};

enum my_position my_current_position;



/////////////////////////////////////

int stop_flag_h=0;
int stop_flag_L=0;
//----------------------------------Sensor_with potentiometer 20K----

int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
// ------------------I/O setup---------------------------
int up_button=12;   // Up button
int down_button=11; // down button
int cal_button=10; // calibrate the hand guide to start position
//------------------------------------------------------

void setup()
{
  Serial.begin(57600);
  pinMode(up_button, INPUT); 
  pinMode(down_button, INPUT);
  pinMode(cal_button, INPUT); 
  pinMode(PIN, OUTPUT);
  pinMode(DIR_C,OUTPUT);
  pinMode(start_point_switch,INPUT);
  pinMode(switch_for_hand_grip,INPUT);
  pinMode(power_off,INPUT);
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
  
  if (digitalRead(power_off)==HIGH)  {
    //EEPROM.write(address, hall_A1_counter);
    EEPROMWriteInt(0, hall_A1_counter);
    delay(300);
  }
//  if (dada==HIGH)  {
//    handgrip_stop_backward();
//    
//    delay(100);
//  }
  
  Serial.println(hall_A1_counter);
  //Serial.println(dada);
 // Serial.println(digitalRead(power_off));
    
  
    //--------------------------------calibration Routin Start here------------------------------------------------------
                                                  if(calibrate==HIGH) 
                                                  {  
                                                    
                                                    if (handguide_position< start_position)
                                                    {
                                                     
                                                      digitalWrite(DIR_C, LOW);
                                                      analogWrite(PWM_C, 30);
                                                                  }
                                                    if (handguide_position>=start_position)
                                                    {
                                                      
                                                      digitalWrite(DIR_C, HIGH);
                                                      analogWrite(PWM_C, 30);
                                                                }
                                                                //if (handguide_position>100 && handguide_position<120 ) analogWrite(PWM_C, 0);
                                                    //if (handguide_position>100 && handguide_position<105 ) analogWrite(PWM_C, 0);
                                                    stop_flag_h=1;
                                                    
                                                    stop_flag_L=1;
                                                   // backward_A();
                                                            }
                                                            
//                                                            if (dada==HIGH)  {
//                                                                        
//                                                                        delay(1000);
//                                                                        stop_flag_h=0;
//                                                                        analogWrite(PWM_C, 0);
//                                                                      }
//                                                            
                                                            //calibrate=LOW;
                                                            //if(calibrate==!calibrate) analogWrite(PWM_C, 0);
                                                   if (handguide_position>start_position && handguide_position<start_position+5 && stop_flag_h==1)
                                                  { 
                                                    //stop();
                                                    analogWrite(PWM_C, 0);
                                                    stop_flag_h=0;
                                                  }
                                                  
                                                  
                                                  if( stop_flag_L==1)
                                                  {     digitalWrite(DIR_A, LOW);
                                                        analogWrite(PWM_A, 200);
                                                        digitalWrite(DIR_B, LOW);
                                                        analogWrite(PWM_B, 200);
                                                    if (digitalRead(start_point_switch)==HIGH)  {
                                                            
                                                           analogWrite(PWM_A, 0);
                                                           analogWrite(PWM_B, 0);
                                                          hall_A1_counter=0;
                                                          stop_flag_L=0;
                                                        }
                                                           
                                                                           }
                                                                           
                                                                           if (digitalRead(start_point_switch)==HIGH)  {
                                                            
                                                           
                                                          hall_A1_counter=0;
                                                          
                                                        }
                                                                          
                                                 
                //*******************************************************Lift start here ***********************************************************
                
                if (forward==HIGH)
               { 
                 //delay(100);
                // Serial.println(forward);
                 
                 switch (my_current_position) {
                    case MY_POSITION_T0:    // your hand is on the sensor
                      Serial.println("T0");
                      if (handguide_position>=60 && handguide_position<=170)
                          {
                                     
                                 if (handguide_position>start_position-10 && handguide_position<=start_position+30) forward_C();// Handgrip is　going  
                                 if (handguide_position<100)
                                    {
                                      forward_A();
                                      forward_B(); // lift　is　going 
                                    }
                                 if (handguide_position<=80) handgrip_stop_forward(); // // handgrip stops
                                               
                                   }
                      break;
                    case MY_POSITION_T1:    // your hand is close to the sensor
                      Serial.println("   T1");
                        // does not used here
                      break;
                    case MY_POSITION_T2:    // 
                      Serial.println("         T2");
                      if (handguide_position>75 && handguide_position<160) backward_C();  // going reverse stops at 80
                      if (handguide_position >=160) 
                      {
                        stop();
                        handgrip_stop_backward();
                       
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
                      //backward==LOW;
                      analogWrite(PWM_A, 0);
                      analogWrite(PWM_B, 0);
                      delay(500);
                      hall_A1_counter=0;
                    }
              
                backward_A();
                backward_B();
                
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
          }

          if (hall_A1_counter>=0&& hall_A1_counter<=1000) my_current_position = MY_POSITION_T0;
          if (hall_A1_counter>1000&& hall_A1_counter<=1300) my_current_position = MY_POSITION_T1;
          if (hall_A1_counter>=2000&& hall_A1_counter<=3000) my_current_position = MY_POSITION_T2;
          
 //delay(200);              
  //Serial.println(handguide_position);       
  Serial.println(hall_A1_counter);
// Serial.println(duty_c_backward);
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
//--------------function Motor A forward-------------------------------------
void forward_A()
{
  digitalWrite(DIR_A, HIGH);
  if (duty_c_forward<255) 
  {
  duty_c_forward++;
  analogWrite(PWM_A, duty_c_forward);
  delay(2);
  }
}

//------------------function Motor A backward forward----
void backward_A()
{
  digitalWrite(DIR_A, LOW);
  if (duty_c_backward<255)
 { 
   duty_c_backward++;
  analogWrite(PWM_A, duty_c_backward);
  delay(2);
 }
}

//------------------function Motor B forward----------------------------------
void forward_B()
{
  digitalWrite(DIR_B, HIGH);
  analogWrite(PWM_B, duty_c_forward);
  delay(2);
}

//------------------function Motor B backward ------
void backward_B()
{
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_B, duty_c_backward);
  delay(2);
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
 
  if (duty_c_forward >= 1) {
    analogWrite(PWM_A, duty_c_forward--);
    analogWrite(PWM_B, duty_c_h_forward--);
    delay(2);
  }
 
  if (duty_c_backward >= 1) {
    analogWrite(PWM_A, duty_c_backward--);
    analogWrite(PWM_B, duty_c_h_backward--);
    delay(2);
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
    delay(1);
  }
}


