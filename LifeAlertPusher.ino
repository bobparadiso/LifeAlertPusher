//running on NodeMCU 1.0
//using WeMo emulator code by Aruna Tennakoonfrom: https://github.com/kakopappa/arduino-esp8266-alexa-multiple-wemo-switch

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"
#include "Servo.h"

#define SERVO_PIN D7

Servo servo;

#define SERVO_UP 900
#define SERVO_DOWN 1200

// prototypes
void setupWiFi();
void setupSwitches();

//on/off callbacks 
void switch1On();
void switch1Off();

// Change these before you flash
//#define FIRST_RUN
const char* ssid = "****";
const char* password = "*********";

UpnpBroadcastResponder upnpBroadcastResponder;
Switch *switch1 = NULL;

//
void lerpServo(uint16_t startPos, uint16_t endPos, uint32_t duration)
{
	servo.writeMicroseconds(startPos);
	//delay(100); //just in case

	uint32_t startTime = millis();
	uint32_t endTime = startTime + duration;

	while (1)
	{
		uint32_t elapsed = millis() - startTime;
		if (elapsed >= duration)
		{
			servo.writeMicroseconds(endPos);
			break;
		}

		uint16_t pos = map(elapsed, 0, duration, startPos, endPos);
		servo.writeMicroseconds(pos);
	}
}

//
void setup()
{
	Serial.begin(9600);
  Serial.println("setup");
	//Serial.setDebugOutput(true);

	servo.attach(SERVO_PIN);
	servo.writeMicroseconds(SERVO_UP);

	setupWiFi();
	setupSwitches();
  Serial.println("ready");
}
 
void loop()
{
	if (WiFi.status() != WL_CONNECTED)
		return;
	
	upnpBroadcastResponder.serverLoop();
	switch1->serverLoop();
}

// connect to wifi – returns true if successful or false if not
void setupWiFi()
{
	boolean state = true;

#ifdef FIRST_RUN
	Serial.println("configuring WiFi chip");
	WiFi.mode(WIFI_STA);
	WiFi.setAutoReconnect(true);	  
	WiFi.begin(ssid, password);
	Serial.println("WiFi chip configured");
#endif
	
	Serial.println("");
	Serial.println("Connecting to WiFi");

	// Wait for connection
	Serial.print("Connecting ...");
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(100);
		Serial.print(".");
	}

	Serial.println("");
	Serial.print("Connected to ");
	Serial.println(ssid);
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
}

//
void setupSwitches()
{
	upnpBroadcastResponder.beginUdpMulticast();

	// Define your switches here. Max 14
	// Format: Alexa invocation name, local port no, on callback, off callback
	switch1 = new Switch("life alert", 80, switch1On, switch1Off);

	Serial.println("Adding switches upnp broadcast responder");
	upnpBroadcastResponder.addDevice(*switch1);
}

//
void triggerLifeAlert()
{
	lerpServo(SERVO_UP, SERVO_DOWN, 500);
	delay(750);
	lerpServo(SERVO_DOWN, SERVO_UP, 500);
}

//trigger life alert either way
void switch1On()
{
	Serial.print("Switch 1 turn on ...");
	triggerLifeAlert();
}

//trigger life alert either way
void switch1Off()
{
	Serial.print("Switch 1 turn off ...");
	triggerLifeAlert();
}
