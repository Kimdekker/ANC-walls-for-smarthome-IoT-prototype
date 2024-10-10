# First things first

Before starting with coding, I first came up with a plan with multiple levels of UI and stages so that I can set up the basics first, and if it works, I can gradually make it cooler :)

Therefore, I created a use case to better define what I will do and, based on that, choose and research the right APIs.

Here is the use case:
![usecase](https://github.com/user-attachments/assets/052e206c-d8e0-4036-9605-86b18488bfde)


1. [Integration with Google Calendar](#integration-with-google-calendar)

2. [IFTTT for Automation](#iftttfor-automation)

3. [Voicecontrol via Google Assistant](#voicecontrol-via-google-assistant)

4. [Realtime Synchronization](#realtime-synchronization)


### What do you need:
- An ESP32
- Internet connection
- Speaker module for the ESP32
- Google Calendar API
- Google Assistant SDK
- Firebase
- Arduino IDE
- VSCode (or another code editor, to set up the server)



# Integration with Google Calendar

Before we will code anything, we are going to create a project in the dashboard for connecting a google API key.

We do that here:
https://console.cloud.google.com/apis/library/apikeys.googleapis.com?hl=en-GB&project=anc-438112

Then, here create your project by naming it, and then clicking on create.
![project](https://github.com/user-attachments/assets/d92206cb-1963-4112-8855-3ae9f1b03e07)

And then, if it's not already, click on enable:
![enable](https://github.com/user-attachments/assets/d6ec2aa1-0b43-4478-a854-1ffda7e6f633)

Now that we have a project, we have to enable the Google Calendar API.
So we go to the enabled API's and services tab and click on the button "+ enable API's and services"
![Untitled](https://github.com/user-attachments/assets/bede7677-ae1e-4f32-9fac-598112e9efc4)

And then type in Calendar. You need this one:
![calendarAPI](https://github.com/user-attachments/assets/fdf59b40-b63e-478c-970e-0975cefe0c7a)

Then click on enable and then, after a short wait, you are here:
![API](https://github.com/user-attachments/assets/868c3ad0-6bcd-4602-b48c-227f28358c0d)

We now have to create credentials, so we click on the create credentials button and make some...

You have to select the google calendar API in the first field,
and then click the second radiobutton. 
![222](https://github.com/user-attachments/assets/d3435241-53e8-407c-ae1a-4e56cd4837b6)

Then fill in the next step (your own preferable stuff)...

And on the last step, just say you're the owner, and then after skip the last optional fields. 
![333](https://github.com/user-attachments/assets/d05dbb13-2995-4797-946b-d815e7a96963)

So you're done setting up the basics. 
Bacause we need the OAuth credentials, we are going back to the credentials tab and make Oauth credentials:
![0auth](https://github.com/user-attachments/assets/2abf0a7c-2287-494f-aec7-bd2fb66320f5)

The first, click on web application, and then give it a name:
![WEBAPP](https://github.com/user-attachments/assets/02d34427-3676-4b8e-9a06-0864eeee89ec)

And then put in this uri:
![localhost300](https://github.com/user-attachments/assets/53432e54-09c7-4e03-b9e2-29cc6970daf9)

Then you're good on here. 
After you click finish, you'll see your credentials.
You'll have to download the client secret:
![download](https://github.com/user-attachments/assets/4a00eeb1-592f-4ae4-b627-98abd17c229e)

In this file you'll then see the keys you need in a JSON file:
- client_id,
- client_secret,
- and redirect_uri (this is the localhost link)

Read the JSON and you'll find what you need. These keys you will need in the next step...


## The coding

I started coding to link my Google Calendar to C++ code for the ESP32. After some research, I found out that it’s best to run a back-end server to properly handle OAuth 2.0. Fortunately, I was the back-end developer for Project Tech last year, so I'm quite comfortable with that.

Code for the back-end: 

First, open VScode and make a new project, and call it: app.js.

After, download npm packages using your terminal:
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

So we need to run this code. 

We can do this by typing the next things into your terminal:
```
cd desktop
```
Now we changes our directory to our desktop. For the ease of this step, I would suggest you put the app.js file in a map on your desktop. But you can put it wherever you want, you just have to change directory trough cd'ing to there. 

So make a map on your desktop, where you store your app.js file in. and then change your derectory from desktop to that map name (we are now in desktop, and from there we will move into your mapname):
```
cd [yourmapname]
```

And then we can let it run by typing this in the terminal:
```
node app.js
```
You'll see in terminal, that you're code is running on your localhost:3000 port.


#### Re-arange code and logic

I'm going to run the server on another port. 3000 is the default port, and if we want this server to run next to other stuff, It's best to run it on another port. I'm running everything on port 4200. So I adjusted all the uri's and everything in googles api desktop, and in the code.

So what we do is going back to the API Oauth settings in the google dashboard, and making some changes at the redirect uri.

We are going to change it to:
```
http://localhost:4200/oauth2callback
```

And also in the code for the server...

```
const { google } = require('googleapis');
const express = require('express');
const app = express();
const port = 4200;

// OAuth2 client setup
const oAuth2Client = new google.auth.OAuth2(
  '[id]', // client id
  '[secret]', // client secret
  'http://localhost:4200/oauth2callback' // redirect URI (must match what you set in Google API console)
);

// Step 1: Route to initiate authentication
app.get('/auth', (req, res) => {
  const authUrl = oAuth2Client.generateAuthUrl({
    access_type: 'offline', // Important to get refresh token
    scope: ['https://www.googleapis.com/auth/calendar.readonly'], // Scope to access Google Calendar
  });
  res.redirect(authUrl); // Redirect to the Google consent page
});

// Step 2: Route to handle OAuth2 callback and get tokens
app.get('/oauth2callback', async (req, res) => {
  const code = req.query.code;

  if (!code) {
    res.status(400).send('Missing authorization code');
    return;
  }

  try {
    // Exchange the authorization code for access and refresh tokens
    const { tokens } = await oAuth2Client.getToken(code);
    oAuth2Client.setCredentials(tokens); // Set the tokens in the OAuth2 client

    // Optionally store tokens for future use
    console.log('Access Token:', tokens.access_token);
    console.log('Refresh Token:', tokens.refresh_token);

    res.send('Authentication successful! You can now access the calendar. <a href="/getGoogleCalendarEvents">Get Calendar Events</a>');
  } catch (err) {
    res.status(500).send('Error retrieving access token');
  }
});

// API to fetch Google Calendar events
app.get('/getGoogleCalendarEvents', async (req, res) => {
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

// Start the server
app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});

```

So now that the server is running, we go to /auth in the browser: http://localhost:4200/auth

We then get redirected from this url to the right login page by google. So there log in.
![loginfail](https://github.com/user-attachments/assets/1f4f1e9f-a940-4008-b932-89f120ecea47)

Then, you'll probably see this. As you can see, our normal user email adress is not yet verificated as a tester user.
So we need to do this in the dashboard and go to the consent screen, and there add a test user.
![addtester](https://github.com/user-attachments/assets/88348b3e-748d-4bbc-8ac8-3f78a12512fe)

You can fill in your own personal info to make yourself a test user.
![email](https://github.com/user-attachments/assets/99e89389-27fe-480e-ab5c-66aa33af2676)

So now, you are a verified test user and can go to the /auth in your localhost, and re-do the process.

And now you get the good screen, you should click the left, smaller button, because we do know this is right...
![yay](https://github.com/user-attachments/assets/06d03495-7648-4888-8b21-1f884084dc3d)

Then click on continue, and now we are back on the main 4200 page. And the message says we are connected to the calendar. Yay!
What we got is a JSON file, we can now do things with that in the C++ code later, but first let's put a calendar event in the calendar, so we can do something with it.

### Add an item to the calendar
With your own google account (the one that you use for google services and the one that you used to create the API thins above), go to the your google calendar and lets make a meeting there. https://calendar.google.com/calendar/u/0/r

Lets for example make a really long meeting for today, so we can read this out on the ESP later.
![meeting](https://github.com/user-attachments/assets/d97c23a0-f967-4d69-99e1-a3ad5fa737d5)

Fun fact. When you refresh the JSON page you just got, you'll see it getting longer. That's really good, because now you know for sure it works.

Now, we can let the ESP32 send HTTP requests to this backend server to get the Google Calendar events we need. 


## ESP32 code

So lets set up the ESP32 code:
ps. make sure you replace SSID and password with your WiFi credentials, and replace the server url with your own. (in this manual it is http://localhost:4200/getGoogleCalendarEvents)...

```
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Replace with your WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Replace with your backend server URL
const char* serverName = "http://localhost:4200/getGoogleCalendarEvents";

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

Whit this code, we fetch the JSON running on our backend server, and printing the data we want in the serial monitor. In this basic code, we are going to fetch all the events listed in my personal agenda, and listing them in the serial monitor.

We now see this error:
Error on HTTP request: -1

This means the request to our server running local does not work. 
This is because we cannot run localhost servers on an ESP, because then the localhost would be on the ESP32. We need our IP adres to replace the localhost. IP adres is the same as calling localhost, but IP adresses are more specific (like in this case, where we need the localhost from the laptop, not the ESP).

> **_NOTE:_**  Here is where we have to re-arange some things to make it actually work for the thing we want to do, because we can't run a server on a localhost connection, because that is device based, and we want 2 devices to communicate with each other over the server. 


#### Re-arange and added the right IP
You can find your IP adress by typing this in the terminal:
```
ifconfig
```

> **_NOTE:_**  Do not search your IP adress on google on a website or something. The IP that you get in the terminal is the one that you need.



**Check Firewall Settings** 

Ensure that your computer's firewall or antivirus software is not blocking incoming connections on the port your Node.js server is using (4200 in your case).
You may need to allow Node.js through your firewall.

**Check Server Status** 

Ensure your Node.js server is running and accessible from other devices on the same network. You can check this by trying to access the API from another device (like your phone or another computer) connected to the same Wi-Fi network using the IP address.

So If you checked this all out, replace the url with your IP adress:
```
const char* serverName = "http://[your IP adress]:4200/getGoogleCalendarEvents";

```

> **_NOTE:_**  Note that the port is written behind with a : (so in this case :4200) . If you have another port, type in yours.


We will need to make a listener that will make the URl available on all IP networks. This makes the server code a bit more general:
```
// Start the server on all network interfaces
app.listen(port, '0.0.0.0', () => {
  console.log(`Server running at http://0.0.0.0:${port}`);
});
```


Also we have to store a token, so we can safely enter the google calendar OAuth policy. The whole code file shoud look like this:
```
const { google } = require('googleapis');
const express = require('express');
const fs = require('fs');
const app = express();
const port = 4200;

const oAuth2Client = new google.auth.OAuth2(
  '[client id]', // Client ID
  '[secret]', // Client Secret
  'http://localhost:4200/oauth2callback' // Redirect URI
);

// Path to store token
const TOKEN_PATH = 'token.json';

// Step 1: Route to initiate authentication
app.get('/auth', (req, res) => {
  const authUrl = oAuth2Client.generateAuthUrl({
    access_type: 'offline', // Important to get refresh token
    scope: ['https://www.googleapis.com/auth/calendar.readonly'], // Scope to access Google Calendar
  });
  res.redirect(authUrl); // Redirect to the Google consent page
});

// Step 2: OAuth2 callback and getting tokens
app.get('/oauth2callback', async (req, res) => {
  const code = req.query.code;

  if (!code) {
    res.status(400).send('Missing authorization code');
    return;
  }

  try {
    // Exchange the authorization code for access and refresh tokens
    const { tokens } = await oAuth2Client.getToken(code);
    oAuth2Client.setCredentials(tokens); // Set the tokens

    // Save tokens for future use
    fs.writeFile(TOKEN_PATH, JSON.stringify(tokens), (err) => {
      if (err) return console.error('Error saving token', err);
      console.log('Token stored to', TOKEN_PATH);
    });

    res.send('Authentication successful! You can now access the calendar. <a href="/getGoogleCalendarEvents">Get Calendar Events</a>');
  } catch (err) {
    console.error('Error retrieving access token:', err);
    res.status(500).send('Error retrieving access token');
  }
});

// Step 3: Fetch Google Calendar events
app.get('/getGoogleCalendarEvents', async (req, res) => {
  try {
    // Load stored token if available
    if (fs.existsSync(TOKEN_PATH)) {
      const token = fs.readFileSync(TOKEN_PATH);
      oAuth2Client.setCredentials(JSON.parse(token));
    } else {
      return res.status(401).send('Token not found, please authenticate at /auth');
    }

    const calendar = google.calendar({ version: 'v3', auth: oAuth2Client });

    // Fetch upcoming events
    const result = await calendar.events.list({
      calendarId: 'primary',
      timeMin: (new Date()).toISOString(),
      maxResults: 10,
      singleEvents: true,
      orderBy: 'startTime',
    });

    res.json(result.data.items); // Send the event data in JSON format
  } catch (err) {
    console.error('Error fetching calendar events:', err);
    res.status(500).send('Error fetching calendar events');
  }
});

// Start the server on all network interfaces
app.listen(port, '0.0.0.0', () => {
  console.log(`Server running at http://0.0.0.0:${port}`);
});


```

We can test if this works by running the server again:

First: "control + C" in terminal, to make the server stop running.

And then:
```
node app.js
```
To rerun the server again

and then uploading the sketch again in Arduino IDE...

> **_NOTE:_**  Make sure you got the right IP adress in the adress as well...

In your serial monitor, you see something like this now:
```
Upcoming Events:
Event: Meeting
Starts at: 2024-10-10T15:00:00+02:00
.....(more events in your calendar)
```

This means we did it! Now we coupled the Google Calendar API to our ESP32, and made a server running on our local IP adress to cross the OAuth wall of Google and het the token to be able to fetch the content in the calendar.


________________________________________________________________________________

# IFTTT for Automation

Now that we have a connection with the Google calendar, we will want to add some conditions to the data, so we can make it smart.
What we are going to do is use IFTTT to define our conditions, and when that condition is met, we will hear it from out our speaker module.

### What is IFTTT
IFTTT stands for If this, then that. So we are going to make some if else statements in our Arduino IDE. This way we can say to the ESP32 > If a new meeting is starting, Then start the Active Noice Cancellation.

## First things first
We need to let our speaker work, before we will couple it with the rest of the project. 
So lets get going...

Put the Speaker wires into the red board (make sure its the right way, see photo)
![IMG_3076](https://github.com/user-attachments/assets/1bceb53a-42a8-4157-b682-3102a9fce0bd)

And then the 4 coloured wires onto the 4 pins from the red board:
![IMG_3077](https://github.com/user-attachments/assets/d6178f09-9ee2-4a08-bee8-64e7164c8f61)

Make sure that it's the good way, so the black wire is on GND. 

Then we can put the wires on the pins of the ESP32:
- Black wire -> GND
- Red wire -> 3V3
- White wire -> RX2
- Yellow wire -> TX2

After we can put a sound on the microSD card. Choose one from the interwebs, or create your own.
Put that sound on your SD card and name it by number first, before you call your name > "001_dafunk"

Then download this library:
https://github.com/harmsel/SensorLab/blob/main/Componenten/MP3_OpenSmart/RedMP3-library.zip

And copy this code:

And paste that code into a new Arduino uno file. 
```
#include "RedMP3.h"

// Define the RX and TX pins
#define MP3_RX 16 // Connect RX of MP3 module to GPIO 16 (RX2)
#define MP3_TX 17 // Connect TX of MP3 module to GPIO 17 (TX2)

// Create an MP3 object with RX and TX pin numbers
MP3 mp3(MP3_RX, MP3_TX);

void setup() {
  // Initialize the Serial Monitor for debugging
  Serial.begin(115200); // Baud rate for the Serial Monitor
  Serial.println("MP3 player setup completed.");
  
  // Initialize Serial2 for communication with the MP3 module
  Serial2.begin(9600);  // Baud rate for the MP3 module
}

void loop() {
  // Play first MP3 file (001_.mp3), set volume to 20 (max is 30)
  mp3.playWithVolume(1, 20);
  delay(5000);  // Wait for 5 seconds
}
```

Add the library by going to: Sketch > Add library > Add .ZIP file... and then pick the zip you downloaded from the github repo.

Then hit the upload button.

