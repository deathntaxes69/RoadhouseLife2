// models/paycheckModel.js
const mongoose = require('mongoose');

const paycheckSchema = new mongoose.Schema({
    characterId: { type: String, required: true, unique: true },
    lastPaycheck: { type: Date, default: Date.now },
    amount: { type: Number, required: true }
});

const Paycheck = mongoose.model('Paycheck', paycheckSchema);

module.exports = Paycheck;