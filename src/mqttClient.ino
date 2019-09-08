
#include <Arduino.h>

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Function prototypes
void subscribeReceive(char* topic, byte* payload, unsigned int length);
 
// Set your MAC address and IP address here
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 37);
 
// Make sure to leave out the http and slashes!
const char* server = "test.mosquitto.org";
 
// Ethernet and MQTT related objects
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);
const char *deviceId = "kg_ard_1";
const char *topicId = "kg_ard_1_topic";

void setup() {
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  
  // Useful for debugging purposes
  Serial.begin(9600);
  
  // Start the ethernet connection
  Ethernet.begin(mac, ip);              
  
  // Ethernet takes some time to boot!
  delay(3000);                          
 
  // Set the MQTT server to the server stated above ^
  mqttClient.setServer(server, 1883);   
  digitalWrite(9, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(7, HIGH);

  // Attempt to connect to the server with the ID "myClientID"
  if (mqttClient.connect("kg_ard_1")) 
  {
    // Establish the subscribe event
    mqttClient.setCallback(subscribeReceive);
    digitalWrite(9, LOW);
    digitalWrite(8, LOW);
    digitalWrite(7, LOW);
    Serial.println("Connection has been established, well done");
  } 
  else 
  {
    Serial.println("Looks like the server connection failed...");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  // This is needed at the top of the loop!
  mqttClient.loop();
 
  // Ensure that we are subscribed to the topic "MakerIOTopic"
  mqttClient.subscribe("kg_ard_1_topic");
 
  // Dont overload the server!
  delay(4000);
}

const int capacity = JSON_OBJECT_SIZE(4);

void subscribeReceive(char* topic, byte* payload, unsigned int length)
{
  // Print the message
  Serial.print("Message: ");
  for(unsigned int i = 0; i < length; i ++)
  {
    Serial.print(char(payload[i]));
  }
 
  // Print a newline
  Serial.println("");

  StaticJsonDocument<capacity> doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(err.c_str());
  }

  int         portNumber  = doc["portNum"];
  String      command     = doc["command"];
  const char* cmdPayload  = doc["payload"];

  int portPwr = command.equalsIgnoreCase("on")?HIGH:LOW;
  digitalWrite(portNumber, portPwr);   // turn the LED on (HIGH is the voltage level)
  Serial.print("Set port number ");
  Serial.print(portNumber);
  Serial.println(" to HIGH");
}
