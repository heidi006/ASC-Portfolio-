#include <Wire.h>

// C++ code
//SENSING SLAVE
const int LDR = A0;
const int tempPin = A1;
volatile byte lightValue = 0;
volatile byte tempValue = 0;

void setup()
{

 Wire.begin(8); // join as a slave 
 Wire.onRequest(sendData);  
 Serial.begin(9600);

}

void loop()
{
	int rawL = analogRead(LDR);
	lightValue = map(rawL, 0, 1023, 0, 255);
	
  	int rawT = analogRead(tempPin);
	
	float voltage = rawT * (5.0 / 1023.0);
	float celsius = (voltage - 0.5) * 100.0; 
	celsius = constrain( celsius, 0, 50);
	tempValue = map((int)celsius, 0, 50, 0, 255);
  
	delay(50);

 
}


void sendData(){
 Wire.write(lightValue);
 Wire.write(tempValue);

}