

/*
  Simple first robot sketch.
 */
//Arduino PWM Speed Control：
int E1 = 6;   
int M1 = 7;
int E2 = 5;                         
int M2 = 4;    
int turntime = 800;

#include <Ping.h>
Ping ping = Ping(13,0,0);
float distance;
float trigger = 10.00;
float rightreading;
float leftreading;
int delayVal = 500;       // duration of blink, in millis (0 means no blinking in effect)
int delayCnt = 0;       // counter to step through the delayVal

#include <Servo.h>  
Servo myservo;  // create servo object to control a servo 
int pos = 90;    // variable to store the servo position 

void setup() {
  // initialize serial communication:
  Serial.begin(9600); 
  pinMode(M1, OUTPUT);   
  pinMode(M2, OUTPUT);
  myservo.attach(9);
}

void loop() {
    
  // read the sensor:
  // c = forward
  // t = stop
  // w = reverse
  // n = right
  // h = left
  if (Serial.available() > 0) {
    int inByte = Serial.read();

    switch (inByte) {
    case 'c':   
      forward();
      break;
    case 't':    
      fullstop();
      break;
    case 'w':    
      reverse();
      break;
    case 'n':    
      right();
      break;
    case 's':    
      distance = pingIt();
      break;      
    case 'h':    
      left();
      break;
    default:
      // stop all:
      fullstop();
    } 
  }
  
  //Now that we got the manual junk out of the way lets see what is in front.

  if (delayVal != 0) {
    // we have a delayVal, so we're Pinging
    delayCnt++;
    // time to Ping?
    if (delayCnt >= 3) {
      distance = scanforward();
      delayCnt = 0;      // restart the delay period
    }  // end of if (delay expired)
  }  //end of if (delay in effect)   
  
  if (distance > trigger){
    forward();
  }
  else
  {
    change_direction();
    delay(50);
  }
}

//This is the the main directional function
void change_direction(){
  fullstop();
  scanleftright();
  if(leftreading < trigger && rightreading < trigger){
    right();
    delay(turntime * 2);
    forward();
  }
  else if(leftreading < rightreading){
    right();
    delay(turntime);
    forward();
  }
  else
  {
    left();
    delay(turntime);
    forward();
  }
  
}

// All motors forward
void forward(){
  int value;
  myservo.write(90);
  for(value = 0 ; value <= 255; value+=5) 
  { 
    digitalWrite(M1,HIGH);   
    digitalWrite(M2, HIGH);       
    analogWrite(E1, value);   //PWM Speed Control
    analogWrite(E2, value);   //PWM Speed Control
    //delay(10); 
  }  
}

// All motors reverse
void reverse(){
    int value;
    for(value = 0 ; value <= 255; value+=5) 
  { 
    digitalWrite(M1,LOW);   
    digitalWrite(M2, LOW);       
    analogWrite(E1, value);   //PWM Speed Control
    analogWrite(E2, value);   //PWM Speed Control
    delay(20); 
  }  

}


void right(){
    int value;
    for(value = 0 ; value <= 255; value+=5) 
  { 
    digitalWrite(M1,HIGH);   
    digitalWrite(M2, LOW);       
    analogWrite(E1, value);   //PWM Speed Control
    analogWrite(E2, value);   //PWM Speed Control
    //delay(60); 
  }  

}

void forwardright(){
    digitalWrite(M1,HIGH);   
    digitalWrite(M2, HIGH);       
    analogWrite(E2, 255);   //PWM Speed Control
    analogWrite(E1, 170);   //PWM Speed Control
    delay(30);
    analogWrite(E1, 255);   //PWM Speed Control
}

void left(){
    int value;
    for(value = 0 ; value <= 255; value+=5) 
  { 
    digitalWrite(M1,LOW);   
    digitalWrite(M2, HIGH);       
    analogWrite(E1, value);   //PWM Speed Control
    analogWrite(E2, value);   //PWM Speed Control
  }  

}

void fullstop(){
    digitalWrite(M1,HIGH);   
    digitalWrite(M2, HIGH);       
    analogWrite(E1, LOW);   //PWM Speed Control
    analogWrite(E2, LOW);   //PWM Speed Control
}

// Ping stuff
float pingIt()
{
  ping.fire();
  distance = ping.inches();
  //delay(10);
  return distance;
}

float read_ping_avg(){
  
  float total = 0;
  total += pingIt();
  total += pingIt();
  total += pingIt();
  
  float avg = total/3;
  
  return avg;
}

void scanleftright()
{
  rightreading = scanright();
  leftreading = scanleft();
  distance = scanforward();
}

float scanright(){
  myservo.write(135);
  delay(345);
  float tmpreading = read_ping_avg();
  myservo.write(90);
  delay(345);
  return tmpreading;
}

float scanleft(){
  myservo.write(45);
  delay(345);
  float tmpreading = read_ping_avg();
  myservo.write(90);
  delay(345);
  return tmpreading;
}

float scanforward(){
  myservo.write(90);
  delay(345);
  float tmpreading = read_ping_avg();
  return tmpreading;
}


