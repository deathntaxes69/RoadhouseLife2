const express = require('express');
const router = express.Router();
const {
    getTexts,
    sendText
} = require('../controllers/textController');

router.get('/texts/:characterId', getTexts);
router.post('/texts', sendText);

module.exports = router;