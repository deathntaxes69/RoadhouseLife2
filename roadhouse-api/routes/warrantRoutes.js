const express = require('express');
const router = express.Router();
const {
    getWarrants,
    createWarrant,
    deleteWarrant
} = require('../controllers/warrantController');

router.get('/warrants/:characterId', getWarrants);
router.post('/warrant', createWarrant);
router.delete('/warrant/:warrantId', deleteWarrant);

module.exports = router;