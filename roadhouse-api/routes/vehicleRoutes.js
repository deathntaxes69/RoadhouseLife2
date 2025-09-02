// routes/vehicleRoutes.js
const express = require('express');
const router = express.Router();
const {
    getVehicles,
    addVehicle,
    updateVehicleState
} = require('../controllers/vehicleController');

router.get('/getvehicles/:cid', getVehicles);
router.post('/addvehicle', addVehicle);
router.get('/updatevehiclestate/:plate/:state/:garageId', updateVehicleState);

module.exports = router;