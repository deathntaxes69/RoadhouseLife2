const express = require('express');
const router = express.Router();
const {
    syncHouses,      // <-- Add this
    getHouses,
    updateHouse,
    createHouse,
    addUpgrade,
    removeUpgrade
} = require('../controllers/houseController');

// This new route will handle the initial synchronization of all houses from the game
router.post('/housing/sync', syncHouses);

router.get('/houses', getHouses);
router.post('/house', createHouse);
router.post('/house/:id', updateHouse);
router.post('/house/:id/upgrades', addUpgrade);
router.delete('/house/:id/upgrades/:upgradeId', removeUpgrade);

module.exports = router;