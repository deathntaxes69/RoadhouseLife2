const mongoose = require('mongoose');

const vehicleSchema = new mongoose.Schema({
    ownerCid: { type: String, required: true },
    prefab: { type: String, required: true },
    plate: { type: String, required: true, unique: true },
    state: { type: Number, default: 0 }, // 0 = Out, 1 = In Garage, 2 = Impounded
    garageId: { type: Number, default: 1 }
}, { timestamps: true });

module.exports = mongoose.model('Vehicle', vehicleSchema);