#include <LiquidCrystal.h> //เรียกใช้ library
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // RS, E, D4, D5, D6, D7 กำหนดขาพินของ arduino ที่ต่อกับ LCD


byte buf[26];   //ตัวแปรสำหรับเก็บค่า pm ขนาด 26 bytes
byte readchar;    //ตัวแปรสำหรับเก็บค่าที่อ่านแต่ละ byte ที่เซนเซอร์ส่งมา
int recieveSum = 0;   //ผลรวมจากการรวม data ที่ได้รับ
int checkSum = 0;   //ผลรวมของ data ที่ส่งจากเซนเซอร์ไป arduino
int counter = 0;    //นับว่าขกำลังอ่านข้อมูล byte ที่เท่าไหร่ที่ส่งมาจากเซนเซอร์
int counterbuf = 0;   //ระบุตำแหน่งข้อมูลใน buf[26]
int PM01Value = 0;        //define PM1.0 value
int PM2_5Value = 0;       //define PM2.5 value
int PM10Value = 0;       //define PM10 value
char mode = 'a';    //บอกชนิดของโหมดของเซนเซอร์
bool toggle = 0; //ตัวแปรสำหรับตั้งค่าให้เซนเซอร์ทำการส่งค่า หาก toggle == 1
char rx_trigger; //ตัวแปรสำหรับตรวจสอบว่าเซนเซอร์กำลังอยู่ในโหมดใด

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
  if( Serial.available() > 0){ //ตรวจสอบว่ามีการป้อนข้อมูลหรือไม่
    rx_trigger = Serial.read();
    Serial.println(rx_trigger);
  }

  //ตรวจสอบว่าคำสั่งที่ป้อนมาคือ การสั่งให้เซนเซอร์ทำงานโหมดใด
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
    //ตรวจสอบเงื่อนไขเพื่อ ทำการส่งและรับค่า pm
    while(mode =='a' || toggle == 1){
      while (!Serial1.available()) {
 
      }
      readchar = Serial1.read();
      if (readchar == 0x42) { //ตรวจสอบหา byte เริ่มต้นของชุดข้อมูลจากเซนเซอร์
        recieveSum = 0;
        counter = 0;
        counterbuf = 0;
      }
      if (counter < 30) { //หา summation สำหรับการเช็คความถูกต้องของข้อมูล
        recieveSum = recieveSum + readchar;
        if (counter > 3) { //first two bytes are for init, 3rd and 4th are for frame size, not needed for data
          buf[counterbuf] = readchar; //เก็บข้อมูล pm ลงในตัวแปร
          counterbuf++;
        }
      }
      if (counter == 30) 
        checkSum = readchar; //last two bytes of frame are for checksum
      if (counter == 31) {
        checkSum = (checkSum << 8) + readchar;
        if (checkSum == recieveSum) //หาก sum ที่บวกกับ sum ที่มาจากเซนเซอร์เท่ากัน แสดงว่าข้อมูลถูกต้องแล้ว
          break; 
      }
      counter++;
    }
  
    //คำนวณค่าจากเซนเซอร์โดยการรวมค่าของแต่ละตัวจากทั้งบิทบนและบิทล่าง
    PM01Value = (buf[6] << 8) + buf[7];
    PM2_5Value = (buf[8] << 8) + buf[9];
    PM10Value = (buf[10] << 8) + buf[11];
  
    //แสดงผลข้อมูลออกทางจอ LCD
    if(mode == 'a' || toggle == 1){
      lcd.setCursor(0, 0);
      lcd.print("PM2.5Value:");
      lcd.print(PM2_5Value);
      lcd.setCursor(0, 1);
      lcd.print("AQI:");
      lcd.print(aqi);
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
