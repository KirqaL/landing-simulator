 //Работа приемника на моем устройстве
#include <Wire.h>                     //  Подключаем библиотеку для работы с шиной I2C
#include <LiquidCrystal_I2C.h>        //  Подключаем библиотеку для работы с LCD дисплеем по шине I2C
LiquidCrystal_I2C lcd(0x27,20,4);     //  Объявляем  объект библиотеки, указывая параметры дисплея (адрес I2C = 0x27, количество столбцов = 16, количество строк = 2)
#include <SPI.h>                      // Подключаем библиотеку для работы с шиной SPI.
#include <nRF24L01.h>                 // Подключаем файл настроек из библиотеки RF24.
#include <RF24.h>                     // Подключаем библиотеку для работы с nRF24L01+.
RF24     radio(9, 53);                // Создаём объект radio для работы с библиотекой RF24, указывая номера выводов модуля (CE, SS).       
int8_t   PotenciometrPin1 = A2;            // Указываем номер вывода, к которому подключен ползунковый потенциометр
int8_t   PotenciometrPin2 = A3;            // Указываем номер вывода, к которому подключен ползунковый потенциометр
int8_t   PotenciometrPin3 = A5;            // Указываем номер вывода, к которому подключен ползунковый потенциометр
const int AzimPin = 12;//Место кнопки азимута
const int AutoPin = 13;//Место основной кнопки
uint16_t Value1 = 0;             // Определяем переменную для значений, считанных с ползункового потенциометра
uint16_t Value2 = 0;             // Определяем переменную для значений, считанных с ползункового потенциометра
uint16_t Value3 = 0;             // Определяем переменную для значений, считанных с ползункового потенциометра
int turn;
int power;
int diff;
int realdiff;
int Roll,Pitch,Yaw;
float x;
float y;
float z;
float      myData[5];
float    enemyData[5];
void setup() {
  //Дисплей                       
   lcd.init(); 
   lcd.backlight();
   lcd.setCursor(3,0);
   lcd.print("AVG FLY TEST");
   lcd.setCursor(3,1);
   lcd.print("VERSION 1.2 ");
   lcd.setCursor(3,2);
   lcd.print("WITHOUT AZIMUTH");
   lcd.setCursor(3,3);
   lcd.print("POWERED BY XZCMP");
   delay(3000);
   pinMode(AzimPin,INPUT_PULLUP);
   pinMode(AutoPin,INPUT_PULLUP);
  Serial.begin(9600);
  radio.begin           ();                                  // Инициируем работу модуля nRF24L01+.
  radio.setChannel      (121);                                // Указываем канал передачи данных (от 0 до 125), 27 - значит передача данных осуществляется на частоте 2,427 ГГц.
  radio.setDataRate     (RF24_1MBPS);                        // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек.
  radio.setPALevel      (RF24_PA_MAX);// Указываем уровень усиления передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm).
  radio.enableAckPayload();
  radio.openWritingPipe (0xAABBCCDD11LL);                                 // Включаем приемник, начинаем прослушивать открытые трубы.
}                                // Включаем приемник, начинаем прослушивать открытые трубы.
void loop() {
  Value1 = analogRead(PotenciometrPin1);
  Value2 = analogRead(PotenciometrPin2);
  Value3 = analogRead(PotenciometrPin3);
  //Serial.println(Value1);
  int turn = map(Value1,0, 1023, 90, 0);
  int power = map(Value2,0, 1023, 0, 255);
  int diff = map(Value3,0, 1023, -50, 50);
   int height = map(Value1,0, 1023, 35, 4);
   int AziFlag = digitalRead(AzimPin);
   int MainFlag = digitalRead(AutoPin);
   myData[0] = MainFlag;
  if (MainFlag == 0){
    lcd.setCursor(0,0);
    lcd.print("                    ");
    myData[1] = height;
    myData[2] = AziFlag;
    if (AziFlag == 0){
      myData[3] = diff;
      lcd.setCursor(10,0);
      lcd.print("+ AZIMUTH");
      }  
    
    
    lcd.setCursor(0,0);
    lcd.print("AVG AUTO");
    lcd.setCursor(0,1);
    lcd.print("                    ");
    lcd.setCursor(0,1);
    lcd.print("SET HEIGHT = ");
    lcd.setCursor(13,1);
    lcd.print(height);
    lcd.setCursor(0,2);
    lcd.print("                   ");
    lcd.setCursor(0,2);
    lcd.print("Azimuth = ");
    lcd.setCursor(12,2);
    lcd.print(diff);
    lcd.setCursor(0,3);
    lcd.print("                    ");
  //   Serial.print("MAINFLAG "); Serial.println(myData[0]);
  //  Serial.print("Height "); Serial.println(myData[1]);
    //  Serial.print("AziFlag "); Serial.println(myData[2]);
    //    Serial.print("diff "); Serial.println(myData[3]);
    
    }
    else{
  myData[0] = MainFlag;
  myData[1] = turn;
  myData[2] = power;
  myData[3] = diff;
    lcd.setCursor(0,0);lcd.print("                    ");
    lcd.setCursor(0,0);
    lcd.print("AVG MANUAL MODE");
    lcd.setCursor(0,1);lcd.print("                    ");
    lcd.setCursor(0,1);
    lcd.print("SERVO = ");
    lcd.setCursor(8,1);
    lcd.print(turn);
    lcd.setCursor(0,2);lcd.print("                    ");
    lcd.setCursor(0,2);
    lcd.print("DIFF = ");
    lcd.setCursor(7,2);
    lcd.print(diff);
    lcd.setCursor(0,3);lcd.print("                    ");
    lcd.setCursor(0,3);
    lcd.print("POWER = ");
    lcd.setCursor(8,3);
    lcd.print(power);
    
    }
  radio.write(&myData, sizeof(myData));           
    if( radio.isAckPayloadAvailable() ){          
        radio.read(&enemyData, sizeof(enemyData));
         int cm = enemyData[0];
         lcd.setCursor(12,3);
         lcd.print("Heig");
         lcd.setCursor(17,3);
         lcd.print(cm);
      Serial.print("КУРС=");   Serial.print(enemyData[1]); Serial.print(", ");
      Serial.print("ТАНГАЖ="); Serial.print(enemyData[2]); Serial.print(", ");
      Serial.print("КРЕН=");   Serial.print(enemyData[3]); Serial.print("\r\n");
    }
}
