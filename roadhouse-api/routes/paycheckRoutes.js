// routes/paycheckRoutes.js
const express = require('express');
const router = express.Router();
const {
    getPaycheck,
    createOrUpdatePaycheck,
} = require('../controllers/paycheckController');

router.get('/paycheck/:characterId', getPaycheck);
router.post('/paycheck', createOrUpdatePaycheck);

module.exports = router;