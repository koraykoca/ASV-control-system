#include <EasyTransfer.h>

EasyTransfer ET;

#define M1_IN1 7
#define M1_IN2 10
#define M2_IN1 8
#define M2_IN2 9
#define EN 11
#define M2_D2 6
#define M1_D2 5
#define VIN 13 //5V

#define power_relay_act 12
#define pump_relay_act 4

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

struct RECEIVE_DATA_STRUCTURE{
int right_value;
int left_value;
volatile boolean power_buttonPressed;
volatile boolean pump_buttonPressed;
};

RECEIVE_DATA_STRUCTURE txdata;

void setup(){
Serial.begin(9600);
pinMode(M1_IN1, OUTPUT);
pinMode(M1_IN2, OUTPUT);
pinMode(M2_IN1, OUTPUT);
pinMode(M2_IN2, OUTPUT);
pinMode(M1_D2, OUTPUT);
pinMode(M2_D2, OUTPUT);
pinMode(EN, OUTPUT);  
digitalWrite(VIN, HIGH);  
pinMode(power_relay_act, OUTPUT);
pinMode(pump_relay_act, OUTPUT);

TCCR0B = TCCR0B & 0b11111000 | 0x02; //change frequency on pwm pins 5&6 to 8kHz
  
ET.begin(details(txdata), &Serial);
}

void loop(){
  //check and see if a data packet has come in. 
  if(ET.receiveData()){
    //this is how you access the variables. [name of the group].[variable name]
    right_value = map(txdata.right_value, 0, 1023, 0, 255);
    right_value = constrain(right_value, 0, 255);
    left_value = map(txdata.left_value, 0, 1023, 0, 255);
    left_value = constrain(left_value, 0, 255);
    
    power_button_state = txdata.power_buttonPressed;
    pump_button_state = txdata.pump_buttonPressed;
    power_relay(power_button_state); 
    pump_relay(pump_button_state);
    rc_control();
    
    Serial.println(right_value);
    Serial.println(left_value);
    Serial.println(power_button_state);
    Serial.println(pump_button_state);
    delay(50);
  } 
  else{
  Serial.println("no radio available");
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
digitalWrite(EN, HIGH);
digitalWrite(M1_IN1, HIGH);
digitalWrite(M1_IN2, LOW);
analogWrite(M1_D2, x);
Serial.print("right forward: ");
Serial.println(x);        
}
        
void m2_reverse(unsigned int y){
digitalWrite(EN, HIGH);
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
digitalWrite(EN, HIGH);
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
digitalWrite(EN, HIGH);
digitalWrite(M2_IN1, LOW);
digitalWrite(M2_IN2, HIGH);
analogWrite(M2_D2, y);
Serial.print("left back: ");
Serial.println(y);  
}
        
void stop_motors(){
digitalWrite(EN, LOW);
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


