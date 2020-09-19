#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
SoftwareSerial NSerial(12,13);
SoftwareSerial MSerial(2,4);

#define WIFI_SSID "AndroidAP"
#define WIFI_PASSWORD "12345678910"
const char* mqttServer = "mqtt.tasikinovasi.id";
const int mqttPort = 1883;
const char* mqttUser = "user";
const char* mqttPassword = "2020";
WiFiClient espClient;
PubSubClient client(espClient);

String temp1;
boolean kirim;
TaskHandle_t Task1;
TaskHandle_t Task2;
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  1  
void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
 NSerial.begin(9600);
 MSerial.begin(9600);

 client.setServer(mqttServer, mqttPort);
  //firebase
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
   Serial.print(".");
   delay(500);
    }
   Serial.println();
   Serial.print("connected: ");
   Serial.println(WiFi.localIP());
   while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (client.connect("Ventilator-1", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
    client.setCallback(callback);
    client.subscribe("sensor/mven");
task1(); 
task2();
}

void loop() {
  // put your main code here, to run repeatedly:
}
void task1()
{
   xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 0 */                  
                    delay(500); //hapus
}

void task2()
{
   xTaskCreatePinnedToCore(
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 1 */
    delay(500); 
}

void Task1code( void * pvParameters ){
  for(;;){
  PublishMQTT();  
  delay(1000); //didieu masang milisna
  }
}  

//Task2code: blinks an LED every 700 ms
void Task2code( void * pvParameters ){
  for (;;){
  SubscribeMQTT();
  delay(1000);
  }
}

void PublishMQTT()
{
  kirim =true;
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject();
  if (root == JsonObject::invalid()){
    kirim =false;
    return;
    }
    if (kirim){
      StaticJsonBuffer<300> jsonBuffer;
      JsonObject& doc = jsonBuffer.createObject();
      doc["oxy"] = root["spo2"];
      doc["bpm"] = root["bpm"];
      doc["debit"] = root["Flow"];
      doc["pre"] = root["Tekanan"];
      
      char JSONmessageBuffer[300];
      doc.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
        Serial.print("Message arrived in topic: ");
        Serial.println();
        Serial.println(JSONmessageBuffer);
        Serial.println();
         if (client.publish("/device/ventilator-1/data", JSONmessageBuffer) == true) {
          Serial.println("Success sending message");
        } else {
          Serial.println("Error sending message");
           esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
           esp_deep_sleep_start();
        }
        Serial.println("---------------------New--------------------");
    }
 delay(1000); //ieu haous
 }
void SubscribeMQTT()
{
   if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
void callback(char* topic, byte* message, unsigned int length) {
  String messageTemp = "";
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  Serial.print(". Message: ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }

  Serial.println();
  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "sensor/mven") {
    Serial.print("Changing output to ");
    if (messageTemp == "")
    {
      return;
    }
    else if (messageTemp == temp1)
    {
      return;
    }
    else if (messageTemp != temp1)
    {
      MSerial.print(messageTemp);
      messageTemp = temp1;
      Serial.println("Mengirim Ke Mekanik");
      
    }
    }
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Ventilator-1", mqttUser, mqttPassword )) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("sensor/mven-1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
