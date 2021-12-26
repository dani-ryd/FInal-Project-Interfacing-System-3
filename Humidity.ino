#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN D4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
const char* ssid = "ASUS_X00RD";
const char* password = "87654321";
const char* mqttUserName = "MQTT Test";
const char* mqttPass = "Z1UP1MEGH2HAY96X";
const char* writeAPIKey = "6LKZ0LSNYXWLW30T";
long channelID = 1618248;

static const char alphanum[] = "0123456789"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz";
WiFiClient client;
PubSubClient mqttClient(client);
const char* server = "mqtt.thingspeak.com";
unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 20L * 1000L;

void reconnect(){
  char clientID[9];

  while(!mqttClient.connected()){
    Serial.print("Attempting MQTT connection. . .");
    for(int i=0;i<8;i++){
      clientID[i] = alphanum[random(51)];
    }
    clientID[8] = '\0';
    if(mqttClient.connect(clientID, mqttUserName, mqttPass)){
      Serial.print("Connected with client ID : ");
      Serial.print(String(clientID));
      Serial.print(", Username: ");
      Serial.print(mqttUserName);
      Serial.print(" ,Password: ");
      Serial.print(mqttPass);
    }else{
      Serial.print("Failed, rc = ");
      Serial.print(mqttClient.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}
void mqttpublish(){
  dht.begin();
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  String data = String("field1 =" + String(t, DEC) + "&field2" + String(h, DEC));
  int length = data.length();
  char msgBuffer[length];
  data.toCharArray(msgBuffer, length+1);
  Serial.println(msgBuffer);

  String topicString = "channels/" + String(channelID) + "/publish/"+String(writeAPIKey);
  length = topicString.length();
  char topicBuffer[length];
  topicString.toCharArray(topicBuffer, length+1);
  mqttClient.publish(topicBuffer, msgBuffer);
  lastConnectionTime = millis();
}

void setup() {
  Serial.begin(9600);
  int status =WL_IDLE_STATUS;

  while(status != WL_CONNECTED){
    status = WiFi.begin(ssid, password);
    delay(5000);
  }

  Serial.println("Connected to wifi");
  mqttClient.setServer(server, 1883);
}

void loop() {
  if(!mqttClient.connected()){
    reconnect();
  }
  mqttClient.loop();
  if(millis() - lastConnectionTime > postingInterval){
    mqttpublish();
  }
}
