const { google } = require('googleapis');
const express = require('express');
const fs = require('fs');
const app = express();
const port = 4200;

const oAuth2Client = new google.auth.OAuth2(
  '[Client ID]', // Client ID
  '[ClientSecret]', // Client Secret
  '[URI]' // Redirect URI
);

const TOKEN_PATH = 'token.json'; // Path to store token

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
