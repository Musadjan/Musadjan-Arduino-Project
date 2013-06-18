


// --------------motor A I/O for Lift---------
const int DIR_A=22;
const int PWM_A=9;
//----------------motor B I/O --------
const int DIR_B=23;
const int PWM_B=10;
//---------------motor C I/O for hand grip-----------
const int DIR_C=50;
const int PWM_C=6;
const int Max_speed_h=100; // maximum speed of hand grip
//-----------------
const int hallPinA_1 = 2;     // A_the number of the hall effect sensor pin
const int hallPinA_2 = 3;    //// A_the number of the hall effect sensor pin, it is digital input only referens direction
// variables will change:
int hall_A1_counter = 0;          // variable for reading the hall sensor status
int hall_A2_counter = 0; 
int PIN=13;
int status=LOW;
int hall_counter=0;
int hall_real=0;
int forward, backward, calibrate;
const int start_position=100;
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

void loop()
{
  
  sensorValue = analogRead(sensorPin);      // Read value from the potentiometer 
  attachInterrupt(0, hall_A, RISING);
  calibrate=digitalRead(cal_button);        //Reading the cal-button
  forward=digitalRead(up_button);           //Reading the forward button 
  backward=digitalRead(down_button);        //Reading the backward button
  float handguide_position = sensorValue/4;
    
                                                  if(calibrate==HIGH) 
                                                  {
                                                    if (handguide_position< start_position)
                                                    {
                                                      
                                                      digitalWrite(DIR_C, HIGH);
                                                      analogWrite(PWM_C, 30);
                                                                  }
                                                    if (handguide_position> start_position)
                                                    {
                                                      
                                                      digitalWrite(DIR_C, LOW);
                                                      analogWrite(PWM_C, 30);
                                                                }
                                                                //if (handguide_position>100 && handguide_position<120 ) analogWrite(PWM_C, 0);
                                                    if (handguide_position>100 && handguide_position<105 ) analogWrite(PWM_C, 0);
                                                            }
                                                            //if(calibrate==!calibrate) analogWrite(PWM_C, 0);
                                                            
                                                  
                                                 
                                                 
                //*******************************************************************************************************************
                
                if (forward==HIGH)
               { 
                 
                 
                 switch (my_current_position) {
  case MY_POSITION_T0:    // your hand is on the sensor
    Serial.println("T0");
    if (handguide_position>=80 && handguide_position<=175 && my_current_position ==MY_POSITION_T0)
                 {
                   
                   //code
                   
                       if (handguide_position<130) forward_C();// Handgrip is　going         
                       if (handguide_position >130) forward_A(); // lift　is　going 
                       if (handguide_position>=175) handgrip_stop(); // // handgrip stops
                             
                 }
    break;
  case MY_POSITION_T1:    // your hand is close to the sensor
    Serial.println("   T1");
    forward_A();
    handgrip_stop();
    break;
  case MY_POSITION_T2:    // your hand is a few inches from the sensor
    Serial.println("         T2");
    forward_A();
    if (handguide_position>=173) backward_C();  // going reverse
    if (handguide_position<=100) 
    {
      stop();
      handgrip_stop();
    }
    break;
  default:    // your hand is nowhere near the sensor
    Serial.println("                       T3");
    break;
  }
  Serial.println(handguide_position);       
  Serial.println(hall_A1_counter);
               }
//                 if (handguide_position>=97 && handguide_position<=175 && my_current_position ==MY_POSITION_T2)
//                 {
//                   
//                   //code
//                   
//                       if (handguide_position<130) forward_C();// Handgrip is　going         
//                       if (handguide_position >130) forward_A(); // lift　is　going 
//                       if (handguide_position>=175) handgrip_stop(); // // handgrip stops
//                             
//                 }
//                 
//                 
//                 if (my_current_position ==MY_POSITION_T1)
//                 {
//                   
//                   //code
//                   backward_C();   // hand grip forward
//                   //forward_A(); //// lift  forward
//                   
//                            
//                       if (handguide_position>=175) handgrip_stop();
//                             
//                 }
//                 
//                 //forward_A();
//                 //attachInterrupt(0, hall_A1plus, FALLING);
//                 //Serial.println("forward");
//                 if (hall_A1_counter>=200) 
//                  {
//                    digitalWrite(PIN, HIGH);
//                          }    
//                               }
   

             if (backward==HIGH)
              {
              //  Serial.println("backward");
                backward_A();
               // attachInterrupt(0, hall_A1minus, FALLING);
                if (hall_A1_counter<=200) 
                {
                  digitalWrite(PIN, LOW);
                       } 
                            }
                            
          if (backward==LOW && forward==LOW)    // if the buttons are released 
          {
            stop();
            handgrip_stop();
            duty_c_forward=0;
            duty_c_backward=0;
          }

          if (hall_A1_counter>=0&& hall_A1_counter<=1000) my_current_position = MY_POSITION_T0;
          if (hall_A1_counter>1000&& hall_A1_counter<=1300) my_current_position = MY_POSITION_T1;
          if (hall_A1_counter>=1300&& hall_A1_counter<=3000) my_current_position = MY_POSITION_T2;
          
                
//   Serial.println(handguide_position);       
//   Serial.println(hall_A1_counter);
  // Serial.println(duty_c_backward);
}

//----------------------------------------------Functions listed below------------------------------------------------------
void hall_A()
{
  
  if (digitalRead(hallPinA_2)==HIGH)
  {
   hall_A1_counter++;
}
else hall_A1_counter--;
}

//------------------function Motor A forward-------------------------------------
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
}

//------------------function Motor B backward ------
void backward_B()
{
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_B, duty_c_backward);
}

//------------------function Motor C forward----------------------------------
void forward_C()
{
  digitalWrite(DIR_C, HIGH);
  if (duty_c_h_forward<Max_speed_h)
 { 
   duty_c_h_forward++;
  analogWrite(PWM_C,duty_c_h_forward);
  delay(2);
 }
}

//------------------function Motor C backward ------
void backward_C()
{
  digitalWrite(DIR_C, LOW);
  if (duty_c_h_backward<Max_speed_h)
 { 
   duty_c_h_backward++;
  analogWrite(PWM_C, duty_c_h_backward);
  delay(2);
 }
}

//------------------Stop A&B Motor ------

void stop()
{
 
  while (duty_c_forward >= 1) {
    analogWrite(PWM_A, duty_c_forward--);
    //analogWrite(PWM_C, duty_c_h_forward--);
    delay(2);
  }
 
  while (duty_c_backward >= 1) {
    analogWrite(PWM_A, duty_c_backward--);
    //analogWrite(PWM_C, duty_c_h_backward--);
    delay(2);
  }
}


void handgrip_stop()
{
 
  if (duty_c_h_forward >= 1) {
    analogWrite(PWM_C, duty_c_h_forward--);
    delay(1);
  }
 
 if (duty_c_h_backward >= 1) {
    analogWrite(PWM_C, duty_c_h_backward--);
    delay(1);
  }
}


