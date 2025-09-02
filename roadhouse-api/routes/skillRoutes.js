const express = require('express');
const router = express.Router();
const {
    fetchSkills,
    updateSkill
} = require('../controllers/skillController');

// Skill Routes
router.get('/skills/:characterId', fetchSkills);
router.post('/skills', updateSkill);

module.exports = router;