#include <Wire.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // RS, E, D4, D5, D6, D7

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
float ax=0, ay=0, az=0, gx=0, gy=0, gz=0;
float AM,VM;

const int buttonPin = 22; // the number of the pushbutton pin 
const int ledPin =  13;      // the number of the LED pin
int buttonState = 0;

void setup(){
 Wire.begin();
 Wire.beginTransmission(MPU_addr);
 Wire.write(0x6B);  // PWR_MGMT_1 register
 Wire.write(0);     // set to zero (wakes up the MPU-6050)
 Wire.endTransmission(true);
 Serial.begin(9600);

 pinMode(ledPin, OUTPUT);
 pinMode(buttonPin, INPUT);

 lcd.begin(16, 2);
}

void loop(){
 mpu_read();
 Serial.print(ax);
 Serial.print(" ");
 Serial.print(ay);
 Serial.print(" ");
 Serial.print(az);
 Serial.print(" ");
 Serial.print(gx);
 Serial.print(" ");
 Serial.print(gy);
 Serial.print(" ");
 Serial.print(gz);
 Serial.print(" ");
 Serial.print(AM);
 Serial.print(" ");
 Serial.print(VM);
 Serial.println();

 lcd.setCursor(0, 0);
 lcd.print(ax);
 lcd.print(" ");
 lcd.print(ay);
 lcd.print(" ");
 lcd.print(az);
 lcd.setCursor(0, 1);
 lcd.print("Normal");
 lcd.blink();
 
 

 if(AM >= 1 && VM >=400){
  lcd.noBlink();
  while(true){
    buttonState = digitalRead(buttonPin);
    lcd.clear();
    delay(500);
    lcd.print("DANGER");
    delay(500);
    analogWrite(ledPin, 50);
//    lcd.clear();
//    lcd.setCursor(0, 0);
//    lcd.print("Danger");
//    delay(500);
    if(buttonState == HIGH){
      analogWrite(ledPin, 0);
      break;
    } 
  }
 }

 
}

void mpu_read(){
 Wire.beginTransmission(MPU_addr);
 Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
 Wire.endTransmission(false);
 Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
 AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
 AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
 AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
 Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
 GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
 GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
 GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

 ax = AcX/16384.00;
 ay = AcY/16384.00;
 az = AcZ/16384.00;
 
 gx = GyX/131.07;
 gy = GyY/131.07;
 gz = GyZ/131.07;

 AM = pow((pow(ax,2)+pow(ay,2)+pow(az,2)),0.5);
 VM = pow((pow(gx,2)+pow(gy,2)+pow(gz,2)),0.5);
}
