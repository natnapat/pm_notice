#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // RS, E, D4, D5, D6, D7


byte buf[26];  //ตัวแปรสำหรับเก็บค่าที่อ่านจากเซนเซอร์ 26 bytes 
byte readchar; //ตัวแปรสำหรับเก็บค่า byte ที่อ่านได้ ณ ช่วงเวลาใดๆ
int recieveSum = 0; //ผลรวมของข้อมูลที่รับมา
int checkSum = 0; //ผลรวมของข้อมูลที่เซนเซอร์ส่งมา
int counter = 0; //จำนวนนับจำนวน byte ที่อ่านได้
int counterbuf = 0; //ตัวระบุตำแหน่งค่าใน buf[26]
int PM01Value = 0;        //define PM1.0 value
int PM2_5Value = 0;       //define PM2.5 value
int PM10Value = 0;       //define PM10 value
char mode = 'a'; //โหมดของเซนเซอร์
bool toggle = 0; //ตัวแปรสำหรับกำหนดค่าให้เซนเซอร์ทำการส่งค่าหาก toggle == 1
char rx_trigger; //ค่าที่มีการป้อนผ่าน serial monitor

//ตั้งค่า serial สำหรับการสื่อสารระหว่างอุปกรณ์
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  lcd.begin(16, 2);
}

// Standby mode. For low power consumption and prolong the life of the sensor.
void sleep()
{
  uint8_t command[] = { 0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73 };
  Serial1.write(command, sizeof(command));
  mode = 's';
}

// Operating mode. Stable data should be got at least 30 seconds after the sensor wakeup from the sleep mode because of the fan's performance.
void wakeUp()
{
  if(mode =='s'){
    uint8_t command[] = { 0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74 };
    Serial1.write(command, sizeof(command));
    mode = 'a';
  }
}

// Active mode. Default mode after power up. In this mode sensor would send serial data to the host automatically.
void activeMode()
{
  uint8_t command[] = { 0x42, 0x4D, 0xE1, 0x00, 0x01, 0x01, 0x71 };
  Serial1.write(command, sizeof(command));
  mode = 'a';
}

// Passive mode. In this mode sensor would send serial data to the host only for request.
void passiveMode()
{
  uint8_t command[] = { 0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70 };
  Serial1.write(command, sizeof(command));
  mode = 'p';
}

// Request read in Passive Mode.
void requestRead()
{
  if (mode == 'p')
  {
    uint8_t command[] = { 0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71 };
    Serial1.write(command, sizeof(command));
    toggle = 1;
  }
}

void loop()
{
  //ตรวจสอบหากมีการป้อนข้อมูลมาทาง serial monitor 
  if( Serial.available() > 0){
    rx_trigger = Serial.read();
    Serial.println(rx_trigger);
  }
//หากมีข้อมูล ให้ตรวจสอบว่าเป็นการเปลี่ยนเป็นโหมดใด จึงเรียกใช้โหมดนั้น
  if(rx_trigger=='a'){
     activeMode();
  }
  else if(rx_trigger=='s'){
    sleep();
  }
  else if(rx_trigger=='w'){
    wakeUp();
  }
  else if(rx_trigger=='p'){
    passiveMode();
  }
  else if(rx_trigger == 'r'){
    requestRead();
  }
    //ตรวจสอบเงื่อนไขให้เซนเซอร์ส่งค่า 
    while(mode =='a' || toggle == 1){
      while (!Serial1.available()) {
 
      }
      readchar = Serial1.read();
      if (readchar == 0x42) { //หาจุดเริ่มต้นที่ของข้อมูลเซนเซอร์ส่งมา
        recieveSum = 0;
        counter = 0;
        counterbuf = 0;
      }
      if (counter < 30) { //หาผลรวมของข้อมูลที่รับมา
        recieveSum = recieveSum + readchar;
        if (counter > 3) { //first two bytes are for init, 3rd and 4th are for frame size, not needed for data
          buf[counterbuf] = readchar;
          counterbuf++;
        }
      }
      if (counter == 30) 
        checkSum = readchar; //last two bytes of frame are for checksum
      if (counter == 31) {
        checkSum = (checkSum << 8) + readchar;
        if (checkSum == recieveSum) //หากผลรวมของข้อมูลที่ได้รับเท่ากับผลรวมที่เซนเซอร์ส่งมา แสดงว่าข้อมูลถูกต้อง การรับข้อมูลเสร็จสิ้น
          break;
      }
      counter++;
    }
  
    PM01Value = (buf[6] << 8) + buf[7];
    PM2_5Value = (buf[8] << 8) + buf[9];
    PM10Value = (buf[10] << 8) + buf[11];
  
  //แสดงผลข้อมูลออกทาง LCD และ Serial
    if(mode == 'a' || toggle == 1){
      lcd.setCursor(0, 0);
      lcd.print("PM2.5Value:");
      lcd.print(PM2_5Value);
      Serial.println(PM2_5Value);
      delay(500);
      lcd.clear();
    }
    else{
      //Serial.print("sensor is in mode ");
      //Serial.println(mode);
    }
    toggle = 0;
    //delay (3000);
    
}
