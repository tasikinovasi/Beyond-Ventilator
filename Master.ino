#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include "MAX30100_PulseOximeter.h"
#define nextion Serial1//19,18 rx tx
#define mekanik Serial2//17,16 rx tx
#define internet Serial3// 15, 14 rx tx
#define REPORTING_PERIOD_MS     1500
#define REPORTING_PERIOD_MS2    2000
byte sensorInt = 0;
int flowsensor = 2;
float konstanta = 4.5; //konstanta flow meter
volatile byte pulseCount;
float debit;
unsigned int flowmlt;
unsigned long oldTime;
// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;
unsigned long interval = 1000; // the time we need to wait
unsigned long previousMillis = 0;
unsigned long interval1 = 30; // the time we need to wait
unsigned long previousMillis1 = 0;
unsigned long oldtime;


uint32_t tsLastReport = 0;
String data_from_display = "";
String temp, temp1;
int oxy, flo, bpm, gflo, vol, preval, gpre, sensorVal;
float  signalval, pre, voltage;
boolean kirim = false;


// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
  Serial.println("Beat!");
}

void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}

void setup()
{
  Serial.begin(9600);
  nextion.begin(9600);
  mekanik.begin(9600);
  internet.begin(9600);
  Serial.println("MEMULAI VENTILATOR");
  pox.begin();
  pox.setOnBeatDetectedCallback(onBeatDetected);

  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH);
  pulseCount = 0;
  debit = 0.0;
  flowmlt = 0;
  oldTime = 0;
  attachInterrupt(sensorInt, pulseCounter, FALLING);
}

void loop()
{
  if (nextion.available()) {
    for (int i = 0 ; i < 20; i++)
    {
      delay(10);
      data_from_display += char (nextion.read());
    }
  }
  for (int i = 0; i < data_from_display.length(); i++)
  {
    if ((data_from_display[i] == 'x' || data_from_display[i] == 'y' || data_from_display[i] == 'a' || data_from_display[i] == 'p' || data_from_display[i] == 'w' || data_from_display[i] == 'o' || data_from_display[i] == 's' || data_from_display[i] == 'j'))
    {
      temp = data_from_display[i];
    }
  }
  for (int i = 0; i < data_from_display.length(); i++)
  {
    if ((data_from_display[i] == '1' || data_from_display[i] == '2' || data_from_display[i] == '3' || data_from_display[i] == '4' || data_from_display[i] == '5' || data_from_display[i] == '6' || data_from_display[i] == '7' || data_from_display[i] == '8' || data_from_display[i] == '9' || data_from_display[i] == '0' || data_from_display[i] == 'v' || data_from_display[i] == 'o' || data_from_display[i] == 'l' || data_from_display[i] == 'p' || data_from_display[i] == 'r' || data_from_display[i] == 'e' || data_from_display[i] == 'b' || data_from_display[i] == 'c' || data_from_display[i] == 't' || data_from_display[i] == 'y' ))
    {
      temp1 += data_from_display[i];
    }

  }
  //===================================================
  if (data_from_display.substring(0, 3) == "vol") {
    Serial.println(temp1);
    mekanik.println(temp1);
    delay(500);
  }
  else if (data_from_display.substring(0, 3) == "pre") {
    Serial.println(temp1);
    mekanik.println(temp1);
    delay(500);
  }
  else if (data_from_display.substring(0, 3) == "bct") {
    Serial.println(temp1);
    mekanik.println(temp1);
    delay(500);
  }

  else if (temp.substring(0, 1) == "s") {
    Serial.println(temp);
    mekanik.println(temp);
  }
  else if (temp.substring(0, 1) == "a") {
    Serial.println(temp);
    mekanik.println(temp);
  }
  else if (temp.substring(0, 1) == "j") {
    Serial.println(temp);
    mekanik.println(temp);
  }
  else if (temp.substring(0, 1) == "w") {
    Serial.println(temp);
    mekanik.println(temp);
  }
  else if (temp.substring(0, 1) == "o") {
    Serial.println(temp);
    mekanik.println(temp);
  }

  else if (temp.substring(0, 1) == "x") {
    if (!pox.begin()) {
      Serial.println("FAILED");
      for (;;);
    } else {
      Serial.println("SUCCESS");
    }
    kirim = true;

  }

  else if (temp.substring(0, 1) == "y")
  {
    kirim = false;
    pox.shutdown();
  }

  if (kirim)
  {

    pox.update();
    dataSensor();
    tampil();
  }
  temp = "";
  temp1 = "";
  data_from_display = "";
}

void dataSensor()
{

  HR();
  flow();
  Serial.println(debit);
  sensorVal = analogRead(A10);
  voltage = (sensorVal * 5.0) / 1024.0;
  pre = (((3.0 * ((float)voltage - 0.475)) * 1000000.0) / 10e5) ;
  if (pre<0){pre=0;
  }
  gpre = map (pre, 0, 4, 0, 300);
  gflo = map (debit, 0, 4, 0, 100);
  kirimJson();
}
void HR()
{

  pox.update();
  // Asynchronously dump heart rate and oxidation levels to the serial
  // For both, a value of 0 means "invalid"
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    Serial.print("Heart rate:");
    bpm = pox.getHeartRate();
    Serial.print(bpm);
    Serial.print("bpm / SpO2:");
    oxy = pox.getSpO2();
    Serial.print(oxy);
    Serial.println("%");
    //
    //tampil();
    tsLastReport = millis();
  }
}
void tampil() {
  unsigned long currentMillis = millis(); // grab current time

  // check if "interval" time has passed (1000 milliseconds)
  if ((unsigned long)(currentMillis - previousMillis) >= interval) {

    String kirim = "add ";
    String kirim1 = "add ";
    //bpm
    Serial.println("Mengirim Data Ke Nextion\n\n");

    nextion.write(0b11111111);
    nextion.write(0b11111111);
    nextion.write(0b11111111);
    nextion.print("n0.val=");
    nextion.print(bpm);
    nextion.write(0b11111111);
    nextion.write(0b11111111);
    nextion.write(0b11111111);

    //oxy
    nextion.print("n1.val=");
    nextion.print(oxy);
    nextion.write(0b11111111);
    nextion.write(0b11111111);
    nextion.write(0b11111111);

    nextion.print(kirim);
    nextion.write(0b11111111);
    nextion.write(0b11111111);
    nextion.write(0b11111111);

    //flow
    nextion.print("t0.txt=\"");
    nextion.print(int(debit));
    nextion.print("\"");
    nextion.write(0b11111111);
    nextion.write(0b11111111);
    nextion.write(0b11111111);

    kirim += 2;
    kirim += ",";
    kirim += 0;
    kirim += ",";
    kirim += gflo;
    nextion.print(kirim);
    nextion.write(0b11111111);
    nextion.write(0b11111111);
    nextion.write(0b11111111);


    nextion.print("t1.txt=\"");
    nextion.print(pre);
    nextion.print("\"");
    nextion.write(0b11111111);
    nextion.write(0b11111111);
    nextion.write(0b11111111);

    kirim1 += 1;
    kirim1 += ",";
    kirim1 += 0;
    kirim1 += ",";
    kirim1 += gpre;
    nextion.print(kirim1);
    nextion.write(0b11111111);
    nextion.write(0b11111111);
    nextion.write(0b11111111);
    nextion.println();
    /*
      nextion.print("t2.txt=\"");
      nextion.print(vol);
      nextion.print("\"");
      nextion.write(0b11111111);
      nextion.write(0b11111111);
      nextion.write(0b11111111);

    */
    previousMillis = millis();
  }
}
void flow () {
  if (millis() - oldTime > REPORTING_PERIOD_MS2)
  {
    detachInterrupt(sensorInt);
    debit = ((1000.0 / (millis() - oldTime)) * pulseCount) / konstanta * 3;
    oldTime = millis();

    pulseCount = 0;

    attachInterrupt(sensorInt, pulseCounter, FALLING);
  }
}
void kirimJson()
{
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  root ["oxy"] = oxy;
  root ["bpm"] = bpm;
  root ["flo"] = flo;
  root ["pre"] = pre;


  root.printTo(internet);

}
