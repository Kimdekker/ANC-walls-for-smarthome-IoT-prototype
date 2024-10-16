#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>  // For time management
#include "RedMP3.h"

#define MP3_RX 17 // Connect RX of MP3 module to GPIO 17 (TX2)
#define MP3_TX 16 // Connect TX of MP3 module to GPIO 16 (RX2)

// Create an MP3 object with RX and TX pin numbers
MP3 mp3(MP3_RX, MP3_TX);

// Replace with your WiFi credentials
const char* ssid = "iPhone van Kim (3)";
const char* password = "esp8266wifi";

// Replace with your backend server URL
const char* serverName = "http://192.168.178.138:4200/getGoogleCalendarEvents";

// Time variables
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600 * 2, 60000); // Adjust for CET/CEST (+2 hours)

// Function declarations
bool isEventHappeningNow(const char* eventStartTime, const char* eventEndTime);
time_t parseISO8601Time(String timeStr);

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected!");

  // Start NTP client to sync time
  timeClient.begin();
  timeClient.update();

  // Fetch Calendar Data
  fetchCalendarData();

  Serial2.begin(9600);  // Baud rate for the MP3 module
}

void loop() {
  // Update the NTP time periodically
  timeClient.update();

  // Call the fetch function periodically, for example, every 5 minutes
  delay(300000);  // Delay for 5 minutes
  fetchCalendarData();
}

void fetchCalendarData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);  // Your backend server address

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String payload = http.getString();

      // Parsing the JSON response
      DynamicJsonDocument doc(4096);
      deserializeJson(doc, payload);

      bool eventHappeningNow = false;  // To track if any event is happening now, standart = false

      Serial.println("Upcoming Events:");
      for (int i = 0; i < doc.size(); i++) {
        const char* eventSummary = doc[i]["summary"];
        const char* eventStart = doc[i]["start"]["dateTime"];
        const char* eventEnd = doc[i]["end"]["dateTime"];

        // Serial.print("Event: ");
        // Serial.println(eventSummary);
        // Serial.print("Starts at: ");
        // Serial.println(eventStart);
        // Serial.print("Ends at: ");
        // Serial.println(eventEnd);

        // Check if the event is happening now
        if (isEventHappeningNow(eventStart, eventEnd)) {
          Serial.println("An event is happening right now: " + String(eventSummary));
          eventHappeningNow = true;  // Set flag to true if any event is happening now

          mp3.playWithVolume(1, 20); // Start playing on 20 volume (max = 30)
          delay(60000);
        }
        else if (!isEventHappeningNow) {
          Serial.println("No events are happening right now.");
          eventHappeningNow = false;
        }
      }


    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpResponseCode);
    }

    http.end();  // Close the connection
  } else {
    Serial.println("WiFi not connected");
  }
}

// Function to check if the event is happening now
bool isEventHappeningNow(const char* eventStartTime, const char* eventEndTime) {
  // Parse the event start and end times into Unix timestamps
  time_t startTime = parseISO8601Time(String(eventStartTime));
  time_t endTime = parseISO8601Time(String(eventEndTime));

  // Get the current time from NTP (in Unix time)
  time_t currentTime = timeClient.getEpochTime();

  // Check if the current time is within the event's start and end times
  return (currentTime >= startTime && currentTime <= endTime);
}

// Function to parse ISO 8601 date-time string (e.g., "2024-10-11T10:30:00+02:00") into Unix timestamp
time_t parseISO8601Time(String timeStr) {
  int year, month, day, hour, minute, second, tz_hour = 0, tz_minute = 0;
  char tz_sign = '+';

  // Extract date, time, and timezone components from the ISO 8601 string
  sscanf(timeStr.c_str(), "%d-%d-%dT%d:%d:%d%c%d:%d", &year, &month, &day, &hour, &minute, &second, &tz_sign, &tz_hour, &tz_minute);

  // Convert to Unix time
  tmElements_t tm;
  tm.Year = year - 1970;  // TimeLib expects years since 1970
  tm.Month = month;
  tm.Day = day;
  tm.Hour = hour;
  tm.Minute = minute;
  tm.Second = second;

  // Calculate the timezone offset in seconds
  long tz_offset = (tz_hour * 3600L) + (tz_minute * 60L);
  if (tz_sign == '-') {
    tz_offset = -tz_offset;
  }

  // Return the event's Unix timestamp adjusted for the timezone offset
  return makeTime(tm) - tz_offset;
}
