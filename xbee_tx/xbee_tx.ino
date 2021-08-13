#include <EasyTransfer.h>

EasyTransfer ET;

#define RIGHT A0
#define LEFT A1

int power_button = 9; //interrupt
int pump_button = 3; //start pump when button state is 0

struct SEND_DATA_STRUCTURE{
  //put your variable definitions here for the data you want to send
  //THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
  int right_value;
  int left_value;
  volatile boolean power_buttonPressed;
  volatile boolean pump_buttonPressed;
};
SEND_DATA_STRUCTURE txdata;

void setup(){
  Serial.begin(9600);
  pinMode(power_button, INPUT);
  pinMode(pump_button, INPUT);
  ET.begin(details(txdata), &Serial);
}

void loop(){
  //this is how you access the variables. [name of the group].[variable name]
  txdata.right_value = analogRead(RIGHT);
  txdata.left_value = analogRead(LEFT);
  txdata.power_buttonPressed = digitalRead(power_button);
  txdata.pump_buttonPressed = digitalRead(pump_button);
  /*Serial.println("power_button");
  Serial.println(txdata.power_buttonPressed);
  Serial.println("pump_button");
  Serial.println(txdata.pump_buttonPressed);
  Serial.println("left_value");
  Serial.println(txdata.left_value);
  Serial.println("right_value");
  Serial.println(txdata.right_value);*/
  //send the data
  ET.sendData();
  delay(45);
}

