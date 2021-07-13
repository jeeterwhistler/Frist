#include "WiFiEsp.h"
#include "SoftwareSerial.h"
#include <PubSubClient.h>
SoftwareSerial Serial1(4, 3); // RX, TX

char ssid[] = "Justin's AP";   // your network SSID (name)
char pass[] = "0983104487";    // your network password
int status = WL_IDLE_STATUS;
char mqttServer[] = "iot.cht.com.tw";
const char* clientId = "qoo.xyz@msa.hinet.net";
const char* userpass = "PKZ2PE7BAEMZZEGSZG";
char subscribeTopic[] = "/v1/device/12482776231/sensor/Switch_1/csv";

WiFiEspClient wifiClient;
PubSubClient client(wifiClient);

void callback(char* topic, byte* payload, unsigned int length) {
  char *p = (char*)payload;
  char *str;
  int element_cnt = 0;
  
  Serial.print("[");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  //check LED topic or not
  if (strcmp(topic,subscribeTopic) == 0){
    while ((str = strtok_r(p, ",", &p)) != NULL){ // delimiter is the semicolon
      element_cnt++;

      //check LED command is 0 or 1
      if (element_cnt == 4){
        if (str[0] == '0'){
          Serial.println("Turn off LED");
          digitalWrite(5, LOW);    // turn the LED off by making the voltage LOW
        }else if (str[0] == '1'){
          Serial.println("Turn on LED");
          digitalWrite(5, HIGH);   // turn the LED on (HIGH is the voltage level)
        }
      }
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId,userpass,userpass)) {
      Serial.println("connected");
       // ... and resubscribe
      client.subscribe(subscribeTopic);
      Serial.print("Subscribe LED topic is:");
      Serial.println(subscribeTopic);
      
      // Once connected, publish an announcement...
      /*client.publish(publishHBTopic, publishHBPayload);
      Serial.print("Public HB payload is:");
      Serial.println(publishHBPayload);*/
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  
  Serial.begin(115200);   // initialize serial for debugging
  Serial1.begin(9600);    // initialize serial for ESP module
  WiFi.init(&Serial1);    // initialize ESP module
    
 while (status != WL_CONNECTED) {
    Serial.print("Connecting to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    delay(10000);
  }  
  
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);

delay(1500);

pinMode(5, OUTPUT);
}
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
 }
