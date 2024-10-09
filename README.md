# First things first

Before starting with coding, I first came up with a plan with multiple levels of UI and stages so that I can set up the basics first, and if it works, I can gradually make it cooler :)

Therefore, I created a use case to better define what I will do and, based on that, choose and research the right APIs.

Here is the use case:
![usecase](https://github.com/user-attachments/assets/052e206c-d8e0-4036-9605-86b18488bfde)


1. [Integration with Google Calendar](#Integration)

2. [IFTTT for Automation](#ifttt)

3. [Voicecontrol via Google Assistant](#voicecontrol)

4. [Realtime Synchronization](#realtime)

_____________________________________________________________

## What do you need:
- An ESP32
- Internet connection
- Speaker module for the ESP32
- Google Calendar API
- Google Assistant SDK
- Firebase
- Arduino IDE
- VSCode (or another code editor, to set up the server)


_____________________________________________________________

## Integration with Google Calendar

Before we will code anything, we are going to create a project in the dashboard for connecting a google API key.

We do that here:
https://console.cloud.google.com/apis/library/apikeys.googleapis.com?hl=en-GB&project=anc-438112

Then, here create your project by naming it, and then clicking on create.
![project](https://github.com/user-attachments/assets/d92206cb-1963-4112-8855-3ae9f1b03e07)

And then, if it's not already, click on enable:
![enable](https://github.com/user-attachments/assets/d6ec2aa1-0b43-4478-a854-1ffda7e6f633)

Then click on manage and you are in the dashboard for your project.


#### The coding

I started coding to link my Google Calendar to C++ code for the ESP32. After some research, I found out that it’s best to run a back-end server (with Node) to properly handle OAuth 2.0. Fortunately, I was the back-end developer for Project Tech last year, so I'm quite comfortable with that.

Code for the back-end: First, download npm packages:
```
 npm install express googleapis
```

Then setting up to server:
ps. You want to use your personal Client id, client secret and uri from the google calendar API to let it run your calendar, and ofcourse, your OAuth 2.0 credentials.

```
const { google } = require('googleapis');
const express = require('express');
const app = express();
const port = 3000;

const oAuth2Client = new google.auth.OAuth2(
  'YOUR_CLIENT_ID', 
  'YOUR_CLIENT_SECRET', 
  'YOUR_REDIRECT_URI'
);

// Set up your OAuth 2.0 credentials
oAuth2Client.setCredentials({
  access_token: 'YOUR_ACCESS_TOKEN',
  refresh_token: 'YOUR_REFRESH_TOKEN',
});

// API to fetch Google Calendar events
app.get('/getGoogleCalendarEvents', (req, res) => {
  const calendar = google.calendar({ version: 'v3', auth: oAuth2Client });

  calendar.events.list({
    calendarId: 'primary', // Primary calendar or any other calendar ID
    timeMin: (new Date()).toISOString(),
    maxResults: 10,
    singleEvents: true,
    orderBy: 'startTime',
  }, (err, result) => {
    if (err) {
      res.status(500).send(err);
    } else {
      res.json(result.data.items); // Send the event data in JSON format
    }
  });
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});

});

```

This server will be hosted locally, so we won’t deploy it to a live, public server. Otherwise, anyone could connect to my Google Calendar… and we definitely don't want that.

Now, your ESP32 will send HTTP requests to this backend server to get the Google Calendar events. 
Here's how we did that:
ps. make sure you replace SSID and password with your WiFi credentials, and replace the server url with your own.

```
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Replace with your WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Replace with your backend server URL
const char* serverName = "http://your-server-ip-or-domain:3000/getGoogleCalendarEvents";

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

  // Fetch Calendar Data
  fetchCalendarData();
}

void loop() {
  // You can call the fetch function periodically, for example, every 5 minutes
  delay(300000); // Delay for 5 minutes
  fetchCalendarData();
}

void fetchCalendarData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);  // Your backend server address

    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + payload);

      // Parsing the JSON response
      DynamicJsonDocument doc(4096);
      deserializeJson(doc, payload);

      Serial.println("Upcoming Events:");
      for (int i = 0; i < doc.size(); i++) {
        const char* eventSummary = doc[i]["summary"];
        const char* eventStart = doc[i]["start"]["dateTime"];

        Serial.print("Event: ");
        Serial.println(eventSummary);
        Serial.print("Starts at: ");
        Serial.println(eventStart);
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

```
