const express = require('express');
const router = express.Router();
const {
    createPlayerLog,
    createCrimeHistoryLog,
} = require('../controllers/logController');

// Define all log-related routes
router.post('/playerlog/:characterId', createPlayerLog);
router.post('/logcrimehistory', createCrimeHistoryLog);

module.exports = router;