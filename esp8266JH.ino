#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <MideaHeatpumpIR.h>       // https://github.com/ToniA/arduino-heatpumpir

IRSenderBitBang irSender(14);     // IR led on ESP8266 digital pin 14 / NODE D5

HeatpumpIR *heatpumpIR = new MideaHeatpumpIR();

// Update these with values suitable for your network.
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "192.168.31.8";

WiFiClient espClient;
PubSubClient client(espClient);
int SwitchedPin = 0; // Rele na digital 03
String switch1;
String strTopic;
String strPayload;

void setup_wifi() {
 Serial.begin(115200);
  delay(100);
 
  // We start by connecting to a WiFi network
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  strTopic = String((char*)topic);

  
  if(strTopic == "ha/switch1")
    {
    switch1 = String((char*)payload);
    if(switch1 == "ON")
      {
        Serial.println("ON1");
        digitalWrite(SwitchedPin, LOW);
       
      }
    else
      {
        Serial.println("OFF1");
        digitalWrite(SwitchedPin, HIGH);
        
      }
    }

if(strTopic == "ha/switch2")
    {
    switch1 = String((char*)payload);
    if(switch1 == "ON")
      {
        Serial.println("ON");
        Serial.println(F("* Sending Power ON"));
      heatpumpIR->send(irSender, POWER_ON, MODE_COOL, FAN_3, 22, VDIR_UP, HDIR_AUTO);
      delay(1000); // 15 seconds, to allow full shutdown
      }
    else
      {
        Serial.println("OFF");
        Serial.println(F("* Sending Power OFF"));
      heatpumpIR->send(irSender, POWER_OFF, MODE_COOL, FAN_3, 22, VDIR_UP, HDIR_AUTO);
      delay(1000); // 15 seconds, to allow full shutdown
      }
    }

    
}
 
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("ha/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void setup()
{
  setup_wifi(); 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(SwitchedPin, OUTPUT);
}
 
void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
