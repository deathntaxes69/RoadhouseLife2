const mongoose = require('mongoose');

const controlZoneSchema = new mongoose.Schema({
    zoneId: { type: Number, required: true, unique: true },
    ownerFaction: { type: String, default: "" },
    taxAmount: { type: Number, default: 0 },
    // Add any other fields you need for control zones
}, { timestamps: true });

module.exports = mongoose.model('ControlZone', controlZoneSchema);