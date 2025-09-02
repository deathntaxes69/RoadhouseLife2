const express = require('express');
const router = express.Router();
const { 
    getZone,
    setOwner,
    setTax
} = require('../controllers/controlZoneController');

// All routes are based on RL_ControlZoneDbHelper.c
router.get('/controlzone/get/:id', getZone);
router.post('/controlzone/setowner/:id/:faction', setOwner);
router.post('/controlzone/settax/:id/:amount', setTax);

module.exports = router;