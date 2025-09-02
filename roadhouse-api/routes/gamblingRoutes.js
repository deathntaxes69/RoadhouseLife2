// routes/gamblingRoutes.js
const express = require('express');
const router = express.Router();
const {
    getGamblingStats,
    updateGamblingStats,
} = require('../controllers/gamblingController');

router.get('/gambling/stats/:characterId', getGamblingStats);
router.post('/gambling/stats', updateGamblingStats);

module.exports = router;