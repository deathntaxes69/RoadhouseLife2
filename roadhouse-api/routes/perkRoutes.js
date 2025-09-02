const express = require('express');
const router = express.Router();
const {
    fetchPerks,
    addPerk,
    removePerk
} = require('../controllers/perkController');

// Perk Routes
router.get('/perks/:characterId', fetchPerks);
router.post('/perks', addPerk);
router.post('/perks/remove', removePerk);

module.exports = router;