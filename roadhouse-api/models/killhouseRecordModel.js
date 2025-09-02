const mongoose = require('mongoose');

const killhouseRecordSchema = new mongoose.Schema({
    mapName: {
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

const KillhouseRecord = mongoose.model('KillhouseRecord', killhouseRecordSchema);
module.exports = KillhouseRecord;