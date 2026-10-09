
#include <Wire.h>
#include <MPU6050.h>


MPU6050 mpu;

char t; //trasnmit character 
char lastCommand = 'S';  //default mode - Stop 
int  prevSpeed = 180;    //just for checking
int  currentSpeed = 180; //default speed 

unsigned long lastSensorSent = 0;
const unsigned long sensorInterval = 500; //time in-between sent readings

unsigned long lastReceivedC = 0;
const unsigned long commandInterval = 1200; //if no command in 1200ms, stop

//RGB pins 
const int redPin = 4;
const int greenPin = 7;
const int bluePin = 8;


//Pin definitions - 4 motors , 2 H-Bridges 
const int m1LeftPin  = 3; 
const int m2RightPin = 5;
const int m3LeftPin  = 6;
const int m4RightPin = 9;


//Sensor Pins
const int readingPin = A0; 


void setup() {
  Serial.begin(9600);

  pinMode(redPin,   OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin,  OUTPUT);
  speedIndicate(currentSpeed); //signals rgb default state 

 
  pinMode(m1LeftPin,  OUTPUT);
  pinMode(m2RightPin, OUTPUT);
  pinMode(m3LeftPin,  OUTPUT);
  pinMode(m4RightPin, OUTPUT);

 
  Wire.begin();
  mpu.initialize();

  if (mpu.testConnection()) { Serial.println("MPU6050 connected");}
  else {Serial.println("MPU6050 connection failed"); }

}



void loop() {
 
  if (Serial.available() > 0){
    lastReceivedC = millis();

    t = Serial.read(); //transmits data over bluetooth 
    Serial.println(t);
    
    bool commandChanged = (t != lastCommand) || (currentSpeed != prevSpeed); //boolean checks previous speed and direction states

    //variable that updates according to different speed modes
    if (t == '1')        {currentSpeed = 100; speedIndicate(currentSpeed);};
    else if (t == '2')   {currentSpeed = 180; speedIndicate(currentSpeed);};
    else if (t == '3')   {currentSpeed = 255; speedIndicate(currentSpeed);};


    if( commandChanged && (t == 'F' || t == 'B' || t == 'L' || t == 'R' || t == 'S')) {  //if command changes, move motors
      
      if (t == 'F') {        //forward 
        Forward(currentSpeed);
        
      } else if (t == 'B'){  //Backwards
        Backward(currentSpeed);

      } else if (t == 'L'){  //Left
        Left(currentSpeed);

      } else if (t == 'R') { //Right
        Right(currentSpeed);

      } else if (t == 'S') { //stop
        Stop();
      }

    lastCommand = t;          //updates t with current value
    prevSpeed = currentSpeed; //updates current speed with the latest chosen speed mode
    }

  }

  unsigned long now = millis();
  
  if (now - lastSensorSent >= sensorInterval) { //checks how often we send the readings
    lastSensorSent = now;                       // if 500ms passed, send a new reading
    sendReadings();
  }

  if (millis() - lastReceivedC > commandInterval && lastCommand != 'S') {
    Stop();
    lastCommand = 'S';
  }
}



//control functions 
void Forward(int speed) {
  analogWrite(m1LeftPin, speed);
  analogWrite(m3LeftPin, 0);
  analogWrite(m2RightPin, speed);
  analogWrite(m4RightPin, 0);

}

void Backward(int speed) {
  analogWrite(m1LeftPin, 0);
  analogWrite(m3LeftPin, speed);
  analogWrite(m2RightPin, 0);
  analogWrite(m4RightPin, speed);

}

void Left(int speed) {
  analogWrite(m1LeftPin, 0);
  analogWrite(m3LeftPin, 0);       //left side fully stopped 
  analogWrite(m2RightPin, speed);  // right side fully forward
  analogWrite(m4RightPin, 0);

}

void Right(int speed) {
  analogWrite(m1LeftPin, speed);  //left side fully forward
  analogWrite(m3LeftPin, 0);
  analogWrite(m2RightPin, 0);
  analogWrite(m4RightPin, 0);     //right side stopped 

}

void Stop() {
  analogWrite(m1LeftPin, 0);
  analogWrite(m2RightPin, 0);
  analogWrite(m3LeftPin, 0);
  analogWrite(m4RightPin, 0);

}

//speed indactor RGB 
void speedIndicate(int speed) {
  if(speed == 100){               //low speed 
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, HIGH);
  } else if (speed == 180){       //medium speed
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, LOW);    
  } else if (speed == 255){      //high speed
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, LOW);     
  }

}


//Battery Sensor
float readBatteryVoltage(){
    int voltValue = analogRead(readingPin);

    float Voltage = (voltValue * 5.0) / 1023.0;

    float batteryVoltage =
        Voltage * (150000.0 + 10000.0) / 10000.0;

    return batteryVoltage;
}

//Current Sensor
float readCurrent(){
    analogReference(INTERNAL);
    delay(2);
    int currentValue = analogRead(readingPin);
    analogReference(DEFAULT);
    delay(2);

    float shuntVoltage = (currentValue * 1.1) / 1023.0;
    return shuntVoltage / 0.1;
    
}

//function to send all readings at once, in one single line
void sendReadings() {

  float battery = readBatteryVoltage();
  float current = readCurrent();

  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); // Read raw data from MPU6050

  float accelerationX =(ax / 16384.0) * 9.80665;
  float accelerationY =(ay / 16384.0) * 9.80665;
  float accelerationZ = (az / 16384.0) * 9.80665;

  float gyroX = gx / 131.0;
  float gyroY = gy / 131.0;
  float gyroZ = gz / 131.0;

    Serial.print("BATTERY:");         Serial.print(battery, 2);
    Serial.print(",CURRENT:");        Serial.print(current, 2);
    Serial.print(",AX:");             Serial.print(accelerationX, 2);
    Serial.print(",AY:");             Serial.print(accelerationY, 2);
    Serial.print(",AZ:");             Serial.print(accelerationZ, 2);

    Serial.print(",GX:");             Serial.print(gyroX, 2);
    Serial.print(",GY:");             Serial.print(gyroY, 2);
    Serial.print(",GZ:");             Serial.println(gyroZ, 2);
    
}


