const express = require('express');
const dotenv = require('dotenv');
const fs = require('fs');
const path = require('path');
const connectDB = require('./config/db');
const { protect } = require('./middleware/authMiddleware');
const { formatForEnfusion } = require('./utils/responseFormatter');

// --- Create a log file stream ---
const logStream = fs.createWriteStream(path.join(__dirname, 'api.log'), { flags: 'a' });

// --- Load All Route Files ---
const characterRoutes = require('./routes/characterRoutes');
const perkRoutes = require('./routes/perkRoutes');
const logRoutes = require('./routes/logRoutes');
const gangRoutes = require('./routes/gangRoutes');
const vehicleRoutes = require('./routes/vehicleRoutes');
const houseRoutes = require('./routes/houseRoutes');
const skillRoutes = require('./routes/skillRoutes');
const controlZoneRoutes = require('./routes/controlZoneRoutes');
const garageRoutes = require('./routes/garageRoutes');
const deliveryJobRoutes = require('./routes/deliveryJobRoutes');
const gamblingRoutes = require('./routes/gamblingRoutes');
const paycheckRoutes = require('./routes/paycheckRoutes');
const { syncHouses } = require('./controllers/houseController');

// --- Basic Setup ---
dotenv.config();
connectDB();
const app = express();

// --- Middleware ---
app.use(express.text({ type: '*/*' }));
app.use((req, res, next) => {
  if (req.body && typeof req.body === 'string') {
    try {
      req.body = JSON.parse(req.body);
    } catch (e) {
      // Not a JSON string, ignore.
    }
  }
  next();
});

// --- Logging and Final Response Middleware (REFACTORED TO PREVENT RECURSION) ---
app.use((req, res, next) => {
    // Keep a reference to the original methods
    const originalJson = res.json;
    const originalSend = res.send;

    // --- Logging ---
    const logEntry = `-----------------------------------------\n=> ${new Date().toISOString()} | Incoming Request: ${req.method} ${req.originalUrl}\n`;
    logStream.write(logEntry);
    console.log(logEntry.trim());
    if (req.body && Object.keys(req.body).length > 0) {
        const requestBodyLog = `   Request Body: ${JSON.stringify(req.body, null, 2)}\n`;
        logStream.write(requestBodyLog);
        console.log(requestBodyLog.trim());
    } else {
        const emptyBodyLog = '   Request Body: Empty\n';
        logStream.write(emptyBodyLog);
        console.log(emptyBodyLog.trim());
    }

    // --- Override res.json to handle all logic in one place ---
    res.json = function(body) {
        const isDirectRestCall = req.originalUrl.startsWith('/fetchcharacters') || 
                                 req.originalUrl.startsWith('/character/') ||
                                 req.originalUrl.startsWith('/insertcharacter') ||
                                 req.originalUrl.startsWith('/gang/') ||
                                 req.originalUrl.startsWith('/controlzone/') ||
                                 req.originalUrl.startsWith('/housing/') ||
                                 req.originalUrl.startsWith('/garage/') ||
                                 req.originalUrl.startsWith('/delivery/') ||
                                 req.originalUrl.startsWith('/perks/') ||
                                 req.originalUrl.startsWith('/skills/');

        if (isDirectRestCall) {
            const responseLog = `<= ${new Date().toISOString()} | Outgoing Response Status: ${res.statusCode} (Bypassed Formatter)\n   Response Body (raw JSON):\n${JSON.stringify(body)}\n`;
            logStream.write(responseLog);
            res.setHeader('Content-Type', 'application/json');
            return originalJson.call(this, body); // Use the ORIGINAL res.json to break the loop
        }

        // It's an EPF call, format it for the stream
        const responseBody = formatForEnfusion(body);
        const formattedLog = `<= ${new Date().toISOString()} | Outgoing Response Status: ${res.statusCode}\n   Formatted Response Body:\n${responseBody}\n`;
        logStream.write(formattedLog);
        res.setHeader('Content-Type', 'text/plain');
        return originalSend.call(this, responseBody); // Use the ORIGINAL res.send to break the loop
    };
    
    // Also override res.send for non-JSON responses
    res.send = function(body) {
        // Since we now handle all JSON through the res.json override,
        // any call to res.send is for simple text or error responses.
        // We can safely log and send them without formatting.
        if (typeof body === 'object') {
            // If an object accidentally gets to res.send, route it through our safe res.json
            return res.json(body);
        }
        
        const responseLog = `<= ${new Date().toISOString()} | Outgoing Response Status: ${res.statusCode} (Raw Send)\n   Response Body (raw):\n${body}\n`;
        logStream.write(responseLog);
        return originalSend.call(this, body);
    };

    next();
});

// --- Routes ---
app.get('/', (req, res) => {
  res.status(200).send('API is running...'); // Use send for simple text
});

app.post('/RootEntityCollection', protect, syncHouses);

// --- Mount All Routes ---
app.use('/', protect, characterRoutes);
app.use('/', protect, perkRoutes);
app.use('/', protect, logRoutes);
app.use('/', protect, gangRoutes);
app.use('/', protect, vehicleRoutes);
app.use('/', protect, houseRoutes);
app.use('/', protect, skillRoutes);
app.use('/', protect, controlZoneRoutes);
app.use('/', protect, garageRoutes);
app.use('/', protect, deliveryJobRoutes);
app.use('/', protect, gamblingRoutes);
app.use('/', protect, paycheckRoutes);

// --- Start Server ---
const PORT = process.env.PORT || 8000;
app.listen(PORT, () => {
  console.log(`Server running on port ${PORT}`);
});