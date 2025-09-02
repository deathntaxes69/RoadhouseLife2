// models/playerLogModel.js
const mongoose = require('mongoose');

const playerLogSchema = new mongoose.Schema(
  {
    characterId: { type: mongoose.Schema.Types.ObjectId, ref: 'Character' },
    logType: { type: String, required: true },
    logData: { type: String, required: true },
  },
  { timestamps: true }
);
module.exports = mongoose.model('PlayerLog', playerLogSchema);