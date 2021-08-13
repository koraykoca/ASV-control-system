#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define CE_PIN 4
#define CSN_PIN 10
#define JOYSTICK_R A0
#define JOYSTICK_L A1

//void stop();

unsigned int right_value = 0;
unsigned int left_value = 0;
int power_button = 9; //interrupt
int pump_button = 3; //start pump when button state is 0

volatile boolean power_buttonPressed;
volatile boolean buttonChange;
volatile boolean pump_buttonPressed;

// Create a Radio
RF24 radio(CE_PIN, CSN_PIN);

//Single radio pipe address for the 2 nodes to communicate
const uint64_t pipe = 0xE8E8F0F0E1LL;

// 4 element array holding Joysticks readings
uint8_t data[4];

void setup(){
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);
  pinMode(power_button, INPUT);
  pinMode(pump_button, INPUT);
  //attachInterrupt(digitalPinToInterrupt(button), stop, LOW);
  delay(1000);
}

void loop(){
  power_buttonPressed = digitalRead(power_button);
  data[0] = power_buttonPressed;
  pump_buttonPressed = digitalRead(pump_button);
  data[1] = pump_buttonPressed;
  left_value = analogRead(JOYSTICK_L);
  data[2] = map(left_value, 0, 1024, 0 , 255); 
  data[2] = constrain(data[2], 0, 255);
  right_value = analogRead(JOYSTICK_R);
  data[3] = map(right_value, 0, 1024, 0 , 255);  
  data[3] = constrain(data[3], 0, 255);
  Serial.println("power_button");
  Serial.println(data[0]);
  Serial.println("pump_button");
  Serial.println(data[1]);
  Serial.println("left_value");
  Serial.println(data[2]);
  Serial.println("right_value");
  Serial.println(data[3]);
    
  bool ok = radio.write(data, sizeof(data));
  delay(75);
} 

/*void stop(){
  buttonPressed = digitalRead(button);
  while (!buttonPressed){
  //digitalWrite(tx_led, LOW);
  buttonChange = digitalRead(button);
  if (buttonChange == HIGH)
  break;
  } 
 }*/
