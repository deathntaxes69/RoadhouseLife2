// models/crimeHistoryModel.js
const mongoose = require('mongoose');

const crimeHistorySchema = new mongoose.Schema(
  {
    characterId: { type: String, required: true },
    officerChargingId: { type: String, required: true },
    charge: { type: String, required: true },
    ticketAmount: { type: Number, required: true },
    jailTimeAmount: { type: Number, required: true },
  },
  { timestamps: true }
);
module.exports = mongoose.model('CrimeHistory', crimeHistorySchema);