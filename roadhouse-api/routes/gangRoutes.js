// routes/gangRoutes.js
const express = require('express');
const router = express.Router();
const {
    fetchAllGangs,
    createGang,
    gangDeposit,
    gangWithdraw
} = require('../controllers/gangController');

router.get('/fetchallgangs', fetchAllGangs);
router.post('/creategang', createGang);
router.post('/gangdeposit/:gangId/:amount', gangDeposit);
router.post('/gangwithdraw/:gangId/:amount', gangWithdraw);

module.exports = router;