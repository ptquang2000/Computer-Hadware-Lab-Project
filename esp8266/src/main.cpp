#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <vector>
#include <algorithm>

const char* ssid = "Computer Network";
const char* pass = "1921681251";
const char* broker = "192.168.0.103";
const char* topic = "valve1/starttime/1";
const char* topic1 = "valve1/starttime/2";
const char* topic2 = "valve1/duration";

#define W_DAY 86400
#define RELAY 4
#define OFFSET 60

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
WiFiClient espClient;
PubSubClient client(espClient);


const int GTM = 7;
std::vector<String> starttime = {"16:15:00", "16:16:00"};
int duration = 30;


// void callback(char* topic, byte* payload, unsigned int length)
// {
//   Serial1.println("Received Messages;");
//   Serial.println(topic);
//   if (String(topic).equals("valve1/starttime/1"))
//     starttime1 = String((char*)payload).toInt();
//   else if (String(topic).equals("valve1/starttime/2"))
//     starttime2 = String((char*)payload).toInt();
//   else if (String(topic).equals("valve1/duration"))
//     duration = String((char*)payload).toInt();
// }

void wifiSetup()
{
  WiFi.begin("Computer Network", "1921681251");

  Serial.print("Connecting");
  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("\nConnecting to "); Serial.println(broker);
    if (client.connect("valve1"))
    {
      Serial.print("\nConnected to "); Serial.println(broker);
      client.subscribe(topic);
      client.subscribe(topic1);
      client.subscribe(topic2);
    } else 
    {
      Serial.println("Trying reconnect again");
      delay(5000);
    }
  }
}

int time_converter(String time)
{
  int hour = time.substring(0,2).toInt();
  int minute = time.substring(3,5).toInt();
  int second = time.substring(6, 8).toInt();
  Serial.println();
  Serial.print(String(hour) + " hour, ");
  Serial.print(String(minute) + " minute, ");
  Serial.print(String(second) + " second");
  Serial.println();
  return hour * 3600 + minute * 60 + second;
}

void execute_task()
{
  int curTime = time_converter(timeClient.getFormattedTime());
  std::vector<int> times;
  for (unsigned int i = 0; i < starttime.size(); ++i)
  {
    int time = time_converter(String(starttime[i]));
    if (time < curTime) times.push_back(W_DAY - curTime + time);
    else times.push_back(time - curTime);
  }

  int minTime = *std::min_element(times.begin(), times.end());
  Serial.println("\nDelay for " + String(minTime));
  delay(minTime * 1000);
  digitalWrite(RELAY, HIGH);
  delay(duration * 1000);
  digitalWrite(RELAY, LOW);

  times.erase(std::remove(times.begin(), times.end(), minTime), times.end());
  int delayTime = 0;
  if (times.size() == 0) {
    delayTime = W_DAY - (OFFSET + duration + minTime);
  } else {
    int nextTime = *std::min_element(times.begin(), times.end());
    delayTime = nextTime - (OFFSET + duration + minTime);
  }
  Serial.println("\nEnter deepslepp, wake up after " + String(delayTime));
  delay(delayTime * 1000);
}

void setup()
{
  Serial.begin(9600);
  Serial.println();

  wifiSetup();
  timeClient.begin();
  timeClient.setTimeOffset(25200);
  pinMode(RELAY, OUTPUT);
  // client.setServer(broker, 1883);
  // client.setCallback(callback);
}

void loop()
{
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // if (!client.connected()) reconnect();
  // client.loop();
  if (duration != 0) 
    execute_task();
}