#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>

#define FIREBASE_HOST "watering-plant-system-default-rtdb.firebaseio.com"
#define API_KEY "AIzaSyB_zu-rVb6E1lGinalWvVw9pAZ6aSMa9DQ"

#define USER_EMAIL "user_abc_123@gmail.com"
#define USER_PASSWORD "123abcuser"

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

String path = "users/user_abc_123";

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>        

#include <Ticker.h>
Ticker ticker;

#include <NTPtimeESP.h>
strDateTime dateTime;
NTPtime NTPch("ch.pool.ntp.org");  

#define RELAY 4

void tick()
{
  int state = digitalRead(BUILTIN_LED);  
  digitalWrite(BUILTIN_LED, !state);  
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
}

void setup() {
  pinMode(RELAY, OUTPUT);

  Serial.begin(115200);
  Serial.println();

  pinMode(BUILTIN_LED, OUTPUT);

  ticker.attach(0.6, tick);

  WiFiManager wifiManager;

  wifiManager.setAPCallback(configModeCallback);

  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }

  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);

	config.host = FIREBASE_HOST;
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

}

void loop() {

	dateTime = NTPch.getNTPtime(7.0, 0);
  char myDate[20] = "";
	char myTime[20] = "";

  if (dateTime.valid){
		Serial.println();	

    byte actualHour = dateTime.hour;      // Gio
    byte actualMinute = dateTime.minute;  // Phut
    byte actualsecond = dateTime.second;  // Giay
    int actualyear = dateTime.year;       // Nam
    byte actualMonth = dateTime.month;    // Thang
    byte actualday = dateTime.day;         // Ngay

		sprintf(myDate, "%04d%02u%02u",actualyear,actualMonth,actualday);
		sprintf(myTime, "%02uh%02um%02us",actualHour,actualMinute,actualsecond);

		int curtime_in_sec = String(actualHour).toInt() * 3600 + String(actualMinute).toInt() * 60 + String(actualsecond).toInt();
		Serial.print("Current time: ");
		Serial.println(curtime_in_sec);
		
		//------------------------------------------------------------------------
		String Path = path + "/" + String(myDate);
		if (Firebase.RTDB.get(&fbdo, Path.c_str())){
			Serial.print("Path: ");
			Serial.println(Path);
			if (fbdo.dataType() == "json"){

				FirebaseJson &json = fbdo.jsonObject();
				size_t len = json.iteratorBegin();
				String key, value = "";
				int type = 0;
				for (size_t i = 0; i < len; i++)
				{
					json.iteratorGet(i, type, key, value);
					Serial.print(i);
					Serial.print(", ");
					Serial.print("Type: ");
					Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
					if (type == FirebaseJson::JSON_OBJECT)
					{
						Serial.print(", Key: ");
						Serial.print(key);
					}
					Serial.print(", Value: ");
					Serial.println(value);

					int start_in_sec = String(key).substring(0,2).toInt() * 3600 + String(key).substring(3,5).toInt() * 60 + String(key).substring(6,8).toInt();
					int duration_in_sec = String(value).substring(0,2).toInt() * 3600 + String(value).substring(3,5).toInt() * 60 + String(value).substring(6,8).toInt();
					Serial.print("Start: ");
					Serial.print(start_in_sec);
					Serial.print(", Duration: ");
					Serial.println(duration_in_sec);

					if (curtime_in_sec >= start_in_sec && curtime_in_sec <= start_in_sec + duration_in_sec && digitalRead(RELAY) == LOW){
						digitalWrite(RELAY, HIGH);
					}
					else if (curtime_in_sec > start_in_sec + duration_in_sec && digitalRead(RELAY) == HIGH){
						digitalWrite(RELAY, LOW);
					}
				}
				json.iteratorEnd();

			}
			else
			{
				Serial.println(fbdo.payload());
			}
		}
		else {
			Serial.print("Error in get, ");
			Serial.println(fbdo.errorReason());
		}
  }
}
