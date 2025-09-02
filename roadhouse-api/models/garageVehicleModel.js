const mongoose = require('mongoose');

const garageVehicleSchema = new mongoose.Schema({
    characterId: {
        type: String,
        required: true,
        ref: 'Character'
    },
    vehicleId: {
        type: String,
        required: true,
        unique: true
    },
    prefab: {
        type: String,
        required: true,
    },
    state: { // e.g., "Stored", "Impounded", "Destroyed"
        type: String,
        required: true,
        default: "Stored"
    },
    mods: {
        type: Object,
        default: {}
    }
}, {
    timestamps: true
});

const GarageVehicle = mongoose.model('GarageVehicle', garageVehicleSchema);
module.exports = GarageVehicle;