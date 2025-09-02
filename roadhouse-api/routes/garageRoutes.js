const express = require('express');
const router = express.Router();
const {
    getGarageVehicles,
    addVehicleToGarage,
    updateVehicleState
} = require('../controllers/garageController');

router.get('/garage/:characterId', getGarageVehicles);
router.post('/garage/add', addVehicleToGarage);
router.post('/garage/update', updateVehicleState);

module.exports = router;