// controllers/gamblingController.js
const GamblingStats = require('../models/gamblingModel');

const getGamblingStats = async (req, res) => {
    try {
        const stats = await GamblingStats.findOne({ characterId: req.params.characterId });
        if (!stats) {
            return res.status(404).json({ message: 'Gambling stats not found' });
        }
        res.json(stats);
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
};

const updateGamblingStats = async (req, res) => {
    try {
        const stats = await GamblingStats.findOneAndUpdate({ characterId: req.body.characterId }, req.body, { new: true, upsert: true });
        res.status(200).json(stats);
    } catch (error) {
        res.status(400).json({ message: error.message });
    }
};

module.exports = {
    getGamblingStats,
    updateGamblingStats,
};