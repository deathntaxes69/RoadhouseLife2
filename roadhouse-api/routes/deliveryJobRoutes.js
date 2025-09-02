// routes/deliveryJobRoutes.js
const express = require('express');
const router = express.Router();
const {
    getDeliveryJob,
    createOrUpdateDeliveryJob,
    removeDeliveryJob,
} = require('../controllers/deliveryJobController');

router.get('/deliveryjob/:id', getDeliveryJob);
router.post('/deliveryjob', createOrUpdateDeliveryJob);
router.delete('/deliveryjob/:id', removeDeliveryJob);

module.exports = router;