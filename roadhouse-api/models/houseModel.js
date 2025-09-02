const mongoose = require('mongoose');

const houseSchema = new mongoose.Schema({
    uniqueId: {
        type: String,
        required: true,
        unique: true // Ensures no duplicate houses and speeds up searches
    },
    prefab: {
        type: String,
        required: true
    },
    buyPrice: {
        type: Number,
        required: true
    },
    sellPrice: {
        type: Number,
        required: true
    },
    ownerCid: {
        type: String,
        ref: 'Character',
        default: null
    },
    isLocked: {
        type: Boolean,
        default: true
    },
    upgrades: {
        type: [String],
        default: []
    },
    position: { // Keeping these in case you place them via script later
        type: [Number]
    },
    rotation: {
        type: [Number]
    }
}, {
    timestamps: true
});

const House = mongoose.model('House', houseSchema);
module.exports = House;