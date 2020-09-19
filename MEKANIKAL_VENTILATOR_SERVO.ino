#include <VarSpeedServo.h>
VarSpeedServo myservo;  // create servo object to control a servo
// a maximum of eight servo objects can be created
const int servoPin = 39; // the digital pin used for the servo


#include <SPI.h>
#include <Wire.h>

String data_from_display = "";
#define nextion Serial3// 15, 14 rx tx
#define esp Serial2 // 17(RX), 16(TX)

String sp = "" ;
String sd = "" ;
int a = 60;
int sps;
int sds;
boolean motor = false;

void setup() {
  Serial.begin(9600);
  nextion.begin(9600);
  esp.begin(9600);
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object
  myservo.write(0, 0, true); // set the intial position of the servo, as fast as possible, wait until done

}

void loop() {
  if (nextion.available()) {
    delay(30);
    while (nextion.available()) {
      data_from_display += char (nextion.read());
    }
    Serial.println(data_from_display);
  }
  else
  {
    Motor();
  }

  ///////////////////////////////////////////////////////////////////////
  if (esp.available()) {
    delay(30);
    while (esp.available()) {
      data_from_display += char (esp.read());
    }
    Serial.println(data_from_display);
  }
  else
  {
    Motor();
  }



  //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  if (data_from_display.substring(0, 1) == "s") //s untuk stop
  { nextion.println("d");
    motor = false;
    sps = 0;
    sds = 0;
    Motor();
    Serial.println ("stop motor");


  }
  if (data_from_display.substring(0, 1) == "a") //o untuk operat
  {   nextion.println("b");
    motor = true;
    sps = 30;
    sds = 60;
    Motor();
    Serial.println ("motor on");
  }
  if (data_from_display.substring(0, 1) == "j") //o untuk operat
  { nextion.println("b");
    motor = true;
    sps = 30;
    sds = 90;
    Motor();
    Serial.println ("motor on");
  }
  if (data_from_display.substring(0, 1) == "w") //o untuk operat
  { nextion.println("b");
    motor = true;
    sps = 30;
    sds = 70;
    Motor();
    Serial.println ("motor on");
  }
  if (data_from_display.substring(0, 1) == "o") //o untuk operat
  { nextion.println("b");
    motor = true;
    Motor();
    Serial.println ("motor on");
  }
  if (data_from_display.substring(0, 1) == "x") //o untuk operat
  {
    motor = true;
    Motor();
    Serial.println ("motor on");
  }
  if (data_from_display.substring(0, 3) == "vol") {
    sp = (data_from_display.substring(3));
    sps = (sp.toInt());
    Serial.println(sps);
  }
  if (data_from_display.substring(0, 3) == "pre") {
    sd = (data_from_display.substring(3));
    sds = (sd.toInt());
    Serial.println(sds);
  }
  if (data_from_display.substring(0, 3) == "bct") {
    sp = (data_from_display.substring(3));
    sps = (sp.toInt());
    Serial.println(sps);
  }
  data_from_display = "";


}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void Motor()
{
  if (motor == true)
  {
    int spsa = sps / 2;
    myservo.write(sds, sps, true);
    myservo.write(0, spsa, true);
    Serial.println("low motor dijalankan");

  }
  else
  {
    myservo.write(0, 0, true);
    //Serial.println("motor Mati");
  }
}


//  myservo.write(180,255,true);
//myservo.write(sds,sps,true);
