#include <Wire.h>
#include <Servo.h>



// C++ code
//MASTER CODE 

Servo shutterServo;

const int HotL = 3;
const int NormalL = 4;
const int ColdL = 5;
const int ServoPin = 9;




void setup()
{
Wire.begin(); //join as a master 
Serial.begin(9600);

pinMode(HotL, OUTPUT);
pinMode(NormalL, OUTPUT);
pinMode(ColdL, OUTPUT);
shutterServo.attach(ServoPin);
shutterServo.write(0);

}

void loop()
{

Wire.requestFrom(8, 2);
if (Wire.available() >= 2) {
    byte lightValue = Wire.read();
    byte tempValue = Wire.read();
   
moveShutter(lightValue);
controlLEDS(tempValue);

Serial.print("Light: "); Serial.print(lightValue);
Serial.print("Temp: "); Serial.println(tempValue);

} 

delay (200); 
}


void moveShutter(byte light) {
// variable "light" acts a a place holder for the bytes received 

  if(light <= 85) {
    
    	shutterServo.write(0); // light --- shutter closed 
		
    
  
  } else if (light < 170 && light > 85) {
	
    	shutterServo.write(90); // medium light --- shutter half opened
		
    
  } else {
     
    	shutterServo.write(180); // dark --- shutter opened 
		
  
  


}
}

void controlLEDS(byte temp){

  if (temp > 170) {
  	digitalWrite(HotL, HIGH);
    digitalWrite(NormalL, LOW);
	digitalWrite(ColdL, LOW);
  } else if (temp > 85){
  	digitalWrite(HotL, LOW);
    digitalWrite(NormalL, HIGH);
	digitalWrite(ColdL, LOW);
  } else {
  	digitalWrite(HotL, LOW);
    digitalWrite(NormalL, LOW);
	digitalWrite(ColdL, HIGH);
  }


}

