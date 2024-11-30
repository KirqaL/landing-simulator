

// На чужом устройстве
/* ОПРЕДЕЛЕНИЕ ПОЛОЖЕНИЯ МОДУЛЯ */     //
#include <Wire.h>                      //
#include <Servo.h>
Servo left;
Servo right; 
#include <iarduino_Position_BMX055.h>  // Подключаем библиотеку iarduino_Position_BMX055 для работы с Trema-модулем IMU 9 DOF.
iarduino_Position_BMX055 sensor(BMX);  // Создаём объект sensor указывая что требуется работать со всеми датчиками модуля.
                                       // Если указать параметр BMA - то объект будет работать только с акселерометром.
                                       // Если указать параметр BMG - то объект будет работать только с гироскопом.
                                       // Если указать параметр BMM - то объект будет работать только с магнитометром.
                                      // Если указать параметр BMX - то объект будет работать со всеми датчиками сразу.

int lefteng = 3;
int righteng = 5; 
int medeng = 6; 
int height;
int aziflag;
int azi;
int power;
int mm;
int leftheight;
int rightheight;


int echoPin = 9; //Пины пока неизвестны
int trigPin = 8;//Пины пока 
int duration, cm; 
#include <nRF24L01.h>
#include <RF24.h>
RF24  radio(7, 10);
float receptionData[5];
float receivingData[5];
void setup(){      
   pinMode(trigPin, OUTPUT); // назначаем trigPin, как выход
   pinMode(echoPin, INPUT);                    
   right.attach(A0);  
   left.attach(A2);
   Serial.begin(9600);               // Инициируем передачу данных в монитор последовательного порта на скорости 9600 бит/сек.
   while(!Serial){}                  // Ждём готовность Serial к передаче данных в монитор последовательного порта.
   
   sensor.begin();                   // Инициируем работу с датчиками объекта sensor.
      delay(3000);// Выполняем калибровку гироскопа после установки нового диапазона измерений или частоты обновления данных.
   //   sensor.setFastOffset(BMA);        // Выполняем калибровку магнитометра после установки нового диапазона измерений или частоты обновления данных. Функция вызывается в цикле в течении указанного времени модуль должен вращаться.
  
   radio.begin           ();
   radio.setChannel      (121);
   radio.setDataRate     (RF24_1MBPS);
   radio.setPALevel      (RF24_PA_MAX);
   radio.enableAckPayload();
   radio.openReadingPipe (1, 0xAABBCCDD11LL);
   radio.startListening  ();
   radio.writeAckPayload (1, &receptionData, sizeof(receptionData) );
}
void loop(){   
if(radio.available()){                                     // Если в буфере приёма имеются принятые данные от передатчика, то ...
     radio.read(&receivingData, sizeof(receivingData));
     sensor.read();
     int flag = receivingData[0];
     long timer1;
     Serial.print("Flag");Serial.println(flag);
     Serial.println(receivingData[1]);
     Serial.println(receivingData[2]);
     Serial.println(receivingData[3]);
     if (flag == 0){
      long timer;
      long timer2;
        height = receivingData[1];
        leftheight = 0.9*height;
        rightheight = 1.1*height;
        aziflag = receivingData[2];
        azi = receivingData[3];
        if (millis() - timer > 150){
        timer = millis();
        analogWrite(lefteng,power);
        analogWrite(righteng,power);
        analogWrite(medeng,power);
      //Задаем эту мощность 
         digitalWrite(trigPin, LOW); 
         delayMicroseconds(2); 
         digitalWrite(trigPin, HIGH); 
         delayMicroseconds(10); 
         digitalWrite(trigPin, LOW); 
         duration = pulseIn(echoPin, HIGH);
         cm = duration / 58;
         int step1;
         if (abs(cm - leftheight) > 10){
            step1 = 3;
            }
         else{
            step1 = 1;
            }
         if (cm < leftheight){
          power = power + step1;
          if (power > 255){
            power = 60;
           // receptionData[1] = "No"; 
          }
         }
          else if (cm > rightheight){
            power = power - step1;
            if (power < 0){
              power = 60;
              }
            }
            }
     }
      else{
        int turn = receivingData[1];
        right.write(0+turn);  
        left.write(180-turn);
        int power = receivingData[2];
        int diff = receivingData[3];
        int powermed = map(power,0,255,1,180);

        float realdiff = (100 - float(abs(diff)))/100;
        if (diff < 0){
            int realpower = round(realdiff*power);
            analogWrite(righteng, realpower);
            analogWrite(lefteng, power);
            int plus = round(float(powermed)*(float(abs(diff)) / 120));    
            analogWrite(medeng,round(powermed+plus));
             }
         else if (diff > 0){
            int realpower = round(realdiff*power);
            analogWrite(lefteng, realpower);
            analogWrite(righteng, power);  
            int plus = round(float(powermed)*(float(abs(diff)) / 120));    
            analogWrite(medeng,round(powermed+plus));
              }
         else{
            analogWrite(righteng, power);
            analogWrite(lefteng, power);
            analogWrite(medeng,powermed);
          }
          
           digitalWrite(trigPin, LOW); 
         delayMicroseconds(2); 
         digitalWrite(trigPin, HIGH); 
         delayMicroseconds(10); 
         digitalWrite(trigPin, LOW); 
         duration = pulseIn(echoPin, HIGH);
         cm = duration / 58;
         
      }
     receptionData[0] = cm;
     float z = sensor.axisZ;
     float x = sensor.axisX;
     float y = sensor.axisY;
     receptionData[1] = z;
     receptionData[2] = x;
     receptionData[3] = y;
     radio.writeAckPayload (1, &receptionData, sizeof(receptionData));
  }
}
