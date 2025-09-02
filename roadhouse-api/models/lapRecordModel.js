const mongoose = require('mongoose');

const lapRecordSchema = new mongoose.Schema({
    trackName: {
        type: String,
        required: true
    },
    characterId: {
        type: String,
        required: true,
        ref: 'Character'
    },
    characterName: {
        type: String,
        required: true
    },
    time: { // Store time in milliseconds
        type: Number,
        required: true
    }
}, {
    timestamps: true
});

const LapRecord = mongoose.model('LapRecord', lapRecordSchema);
module.exports = LapRecord;