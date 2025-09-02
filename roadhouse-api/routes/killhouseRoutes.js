const express = require('express');
const router = express.Router();
const {
    getKillhouseRecords,
    createKillhouseRecord
} = require('../controllers/killhouseController');

router.get('/killhouse/records/:mapName', getKillhouseRecords);
router.post('/killhouse/record', createKillhouseRecord);

module.exports = router;