#include <FS.h>
#include <ArduinoJson.h> 

#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>

#define FIREBASE_HOST "watering-plant-system-default-rtdb.firebaseio.com"
#define API_KEY "AIzaSyB_zu-rVb6E1lGinalWvVw9pAZ6aSMa9DQ"

// #define USER_EMAIL "user_abc_123@gmail.com"
// #define USER_PASSWORD "123abcuser"

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

// String path = "users/user_abc_123";
String path = "users/";

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>        

#include <Ticker.h>
Ticker ticker;

#include <NTPtimeESP.h>
strDateTime dateTime;
NTPtime NTPch("ch.pool.ntp.org");  

#define RELAY 4

//flag for saving data
bool shouldSaveConfig = false;
char user_email[40];
char user_password[40];

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

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

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setup() {
  pinMode(RELAY, OUTPUT);

  Serial.begin(115200);
  Serial.println();

  pinMode(BUILTIN_LED, OUTPUT);

  ticker.attach(0.6, tick);

	// clean FS, for testing
  SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(user_email, json["user_email"]);
					strcpy(user_password, json["user_password"]);
        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read
  
  WiFiManagerParameter custom_user_email("user_email", "user_email", user_email, 40);
  WiFiManagerParameter custom_user_password("user_password", "user_password", user_password, 40);

  WiFiManager wifiManager;
	// wifiManager.resetSettings();

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

	wifiManager.addParameter(&custom_user_email);
	wifiManager.addParameter(&custom_user_password);
	wifiManager.setCustomHeadElement("<script>document.getElementById('user_password').setAttribute('type','password')</script>");

  wifiManager.setAPCallback(configModeCallback);

  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }

  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, LOW);

	strcpy(user_email, custom_user_email.getValue());
	strcpy(user_password, custom_user_password.getValue());

  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["user_email"] = user_email;
    json["user_password"] = user_password;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

	config.host = FIREBASE_HOST;
  config.api_key = API_KEY;

  auth.user.email = user_email;
  auth.user.password = user_password;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

	path = path + getValue(user_email,'@',0);

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
