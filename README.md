# First things first

Voor ik ben begonnen met code heb ik eerst een plan uitgedacht met meerdere levels of UI en niveaus, zodat ik eerst de basics kan opzetten, en als het lukt steeds een beetje cooler kan zijn :)

Daarom heb ik een Use case gemaakt, zodat ik wat beter kan definieren wat ik ga doen en op basis daarvan juiste API's kiezen en onderzoeken. 

Hier de Use case:
![usecase](https://github.com/user-attachments/assets/052e206c-d8e0-4036-9605-86b18488bfde)


1. Integratie met Google Calendar

2. IFTTT voor Automatisering

3. Spraakbediening via Google Assistant

4. Realtime Synchronisatie:

_____________________________________________________________

Ik ben met code begonnen om mijn google calendar te koppelen aan c++ code voor de ESP32, en na wat research erachter gekomen dat ik het best een back-end server (met node) kan laten draaien om de OAuth 2.0 goed te laten werken. Gelukkig was ik de back-ender met project tech vorig jaar, dus kan ik dat wel redelijk.&nbsp;

Code voor de back-end:
eerst npm packages downloaden
```
 npm install express googleapis
```

Dan de server opzetten:

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

Daarna
