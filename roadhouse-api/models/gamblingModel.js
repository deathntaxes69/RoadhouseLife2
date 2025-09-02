// models/gamblingModel.js
const mongoose = require('mongoose');

const gamblingStatsSchema = new mongoose.Schema({
    characterId: { type: String, required: true, unique: true },
    totalWinnings: { type: Number, default: 0 },
    totalLosses: { type: Number, default: 0 },
    gamesPlayed: { type: Number, default: 0 }
});

const GamblingStats = mongoose.model('GamblingStats', gamblingStatsSchema);

module.exports = GamblingStats;