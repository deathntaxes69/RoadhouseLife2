const express = require('express');
const router = express.Router();
const {
    getLapRecords,
    createLapRecord
} = require('../controllers/racingController');

router.get('/racing/records/:trackName', getLapRecords);
router.post('/racing/record', createLapRecord);

module.exports = router;