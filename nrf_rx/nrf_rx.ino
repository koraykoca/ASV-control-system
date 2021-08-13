#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define CE_PIN  4
#define CSN_PIN 5
#define M1_IN1 7
#define M1_IN2 0
#define M2_IN1 8
#define M2_IN2 6
// #define EN 0
#define M2_D2 10
#define M1_D2 9

#define power_relay_act 2
#define pump_relay_act 3

#define rul 140
#define rll 120
#define lul 135
#define lll 115

volatile boolean power_button_state = 0;
volatile boolean pump_button_state = 0;
unsigned int right_value = 0;
unsigned int left_value = 0;

//functions declaring
void rc_control();
void m2_forward(unsigned int);
void m2_reverse(unsigned int);
void m2_stop();
void m1_forward(unsigned int);
void m1_reverse(unsigned int);
void m1_stop();
void stop_motors();
void power_relay(volatile boolean);
void pump_relay(volatile boolean);

// Define the transmit pipe
const uint64_t pipe = 0xE8E8F0F0E1LL;

// Create a Radio
RF24 radio(CE_PIN, CSN_PIN); 

// 2 element array holding Joystick readings
uint8_t data_received[4];

void setup(){
  pinMode(M1_IN1, OUTPUT);
  pinMode(M1_IN2, OUTPUT);
  pinMode(M2_IN1, OUTPUT);
  pinMode(M2_IN2, OUTPUT);
  pinMode(M1_D2, OUTPUT);
  pinMode(M2_D2, OUTPUT);
  //pinMode(EN, OUTPUT);
  
  pinMode(power_relay_act, OUTPUT);
  pinMode(pump_relay_act, OUTPUT);
  
  Serial.begin(9600);
  stop_motors();

  //TCCR0B = TCCR0B & 0b11111000 | 0x02; //change frequency on pwm pins 5&6 to 8kHz
  //TCCR1B = TCCR1B & 0b11111000 | 0x04; //change frequency on pwm pins 9&10 to 488.28

  //Begin operation of the chip.
  radio.begin();

  // This simple sketch opens a single pipes for these two nodes to communicate
  // back and forth.  One listens on it, the other talks to it.
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  delay(1000);
}

void loop(){
    
  if (radio.available()){
    // Read the data payload until we've received everything
    bool done = false;
      while (!done){
       // Fetch the data payload
       for(int k = 0; k<= 5 ; k++){
       done = radio.read(data_received, sizeof(data_received));
       power_button_state += data_received[0];
       pump_button_state += data_received[1];
       left_value += data_received[2];
       right_value += data_received[3];
       }
        
       power_relay(power_button_state / 5);
       rc_control;
       pump_relay(pump_button_state / 5);
       
       Serial.println("power_button_state");
       Serial.println(power_button_state);
       Serial.println("pump_button_state");
       Serial.println(pump_button_state);
       Serial.println("left_value");
       Serial.println(left_value);
       Serial.println("right_value");
       Serial.println(right_value);
       delay(200);
    }
  }
    else {
    Serial.println("No radio available");
    delay(100);
    stop_motors();
    } 
}
  
void power_relay(volatile boolean r){
  if (r == HIGH){
  digitalWrite(power_relay_act, LOW); 
  }
  else
  digitalWrite(power_relay_act, HIGH);
}

void pump_relay(volatile boolean r){
  if (r == HIGH){
  digitalWrite(pump_relay_act, HIGH); 
  }
  else
  digitalWrite(pump_relay_act, LOW);
}

void m2_forward(unsigned int x){
//digitalWrite(EN, HIGH);
digitalWrite(M1_IN1, HIGH);
digitalWrite(M1_IN2, LOW);
analogWrite(M1_D2, x);
Serial.print("right forward: ");
Serial.println(x);        
}
        
void m2_reverse(unsigned int y){
//digitalWrite(EN, HIGH);
digitalWrite(M1_IN1, LOW);
digitalWrite(M1_IN2, HIGH);
analogWrite(M1_D2, y); 
Serial.print("right back ");
Serial.println(y);   
}

void m2_stop(){
digitalWrite(M1_IN1, LOW);
digitalWrite(M1_IN2, LOW);
analogWrite(M1_D2, 0);  
Serial.println("right stop");
}

void m1_forward(unsigned int y){
//digitalWrite(EN, HIGH);
digitalWrite(M2_IN1, HIGH);
digitalWrite(M2_IN2, LOW);
analogWrite(M2_D2, y);
Serial.print("left forward: ");
Serial.println(y);   
}

void m1_stop(){
digitalWrite(M2_IN1, LOW);
digitalWrite(M2_IN2, LOW);
analogWrite(M2_D2, 0);
Serial.println("left stop");
}
      
void m1_reverse(unsigned int y){
//digitalWrite(EN, HIGH);
digitalWrite(M2_IN1, LOW);
digitalWrite(M2_IN2, HIGH);
analogWrite(M2_D2, y);
Serial.print("left back: ");
Serial.println(y);  
}
        
void stop_motors(){
//digitalWrite(EN, LOW);
Serial.println("stop");
m1_stop();
m2_stop();  
}
       
       void rc_control(){
       if(right_value <= rul && right_value >= rll && left_value <= lul && left_value >= lll){
        stop_motors();
        }
        
        if (right_value > rul){
         unsigned int updated_right = map(right_value, rul + 1, 255, 0 , 255);
         unsigned int y2 = updated_right;
         if (y2 > 240){
         m2_forward(255);
         }
         else{
         m2_forward(y2);
         }
         if (left_value <= lul && left_value >= lll){
          digitalWrite(M2_D2, LOW);
         }
        }
         
         else if (right_value < rll){
         unsigned int updated_right = map(right_value, 0, rll - 1, 0 , 255);
         if (updated_right < 20){
          m2_reverse(255);
         }
         else{
         m2_reverse(255 - updated_right);
         }
         if(left_value <= lul && left_value >= lll){
          digitalWrite(M2_D2, LOW);
         }
       }
         
       if (left_value > lul){
         unsigned int updated_left = map(left_value, lul + 1, 255, 0 , 255);
         //unsigned int y1 = updated_left;
         if (updated_left > 240){
         m1_forward(255);
         }
         else{
         m1_forward(updated_left);
         }
         if(right_value <= rul && right_value >= rll){
          digitalWrite(M1_D2, LOW);
         }
        }
         
       else if (left_value < lll){
         unsigned int updated_left = map(left_value, 0, lll - 1, 0 , 255);
         if(left_value < 20){
         m1_reverse(255);
         }
         else{
         m1_reverse(255 - updated_left);
         }
         if(right_value <= rul && right_value >= rll){
          digitalWrite(M1_D2, LOW);
         }
      }
   }


