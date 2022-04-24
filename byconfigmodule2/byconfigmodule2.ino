HC05 Configuration:
 
#include <SoftwareSerial.h>
#define tx 2
#define rx 3
SoftwareSerial configBt(rx, tx); // RX, TX
void setup() 
{
  Serial.begin(38400);
  configBt.begin(38400);
  pinMode(tx, OUTPUT);
  pinMode(rx, INPUT);
}
void loop() 
{
  if(configBt.available()) //if the bluetooth module is sending something...
  {
    Serial.print(configBt.readString()); //print whatever the bluetooth module is sending 
  }
   
  if(Serial.available()) //if we have typed anything into the serial monitor input text box...
  {
    configBt.write(Serial.read()); //write whatever we typed into the serial monitor input text box to the bluetooth module
  }
}
 
 
Receiver:
 
#include <SoftwareSerial.h>
#define tx 2
#define rx 3
SoftwareSerial bt(rx, tx); //RX, TX
void setup()
{
  Serial.begin(9600);
  bt.begin(9600);
  pinMode(tx, OUTPUT);
  pinMode(rx, INPUT);
}
void loop()
{
  if(bt.available()>0)
  {
    Serial.println(bt.read());
  }
}
 
 
Sender:
 
#include <SoftwareSerial.h>
#define tx 2
#define rx 3
SoftwareSerial bt(rx,tx); //RX, TX
void setup() 
{
  Serial.begin(9600);
  bt.begin(9600);
  pinMode(tx, OUTPUT);
  pinMode(rx, INPUT);
}
void loop() 
{
  bt.write(123);
}
