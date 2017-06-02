#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "wifi-ssid";
const char* password = "wifiPassword";

const char* mqtt_server = "host.org";
const char* mqtt_username = "username";
const char* mqtt_password = "password";
char  mqtt_pub_topic[50];
int mqtt_pub_interval = 10000;
byte mac[6];

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char sensorValueMeasurement[150];
String myMacAddress;

const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
 
int sensorValue = 0;  // value read from the pot
int outputValue = 0;

void setup_wifi() {
   delay(100);
  // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  
  WiFi.macAddress(mac);
  myMacAddress = mac2String(mac);
  sprintf (mqtt_pub_topic, "sensors/home/gortz/nodemcu/%s/sensors", myMacAddress.c_str());
}

String mac2String(byte ar[]){
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%2X", ar[i]);
    s = buf+s;
  }
  s.toLowerCase();
  return s;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str(),mqtt_username,mqtt_password))
    {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 6 seconds before retrying
      delay(6000);
    }
  }
} 

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  Serial.print("Connected to mqtt");
}

void loop() {
   if (!client.connected()) {
    reconnect();
  }
  sensorValue = analogRead(analogInPin);
  outputValue = map(sensorValue, 0, 512, 0, 100);    

  sprintf (sensorValueMeasurement, "[{\"bn\" :\" %s \"}, {\"n \" : \"tellstick;soil_moisture\", \"v\" : %i}]",myMacAddress.c_str(),outputValue);  
  client.publish(mqtt_pub_topic, sensorValueMeasurement);
  delay(mqtt_pub_interval);
  }
