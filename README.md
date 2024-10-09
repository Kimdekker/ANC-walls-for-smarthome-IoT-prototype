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
