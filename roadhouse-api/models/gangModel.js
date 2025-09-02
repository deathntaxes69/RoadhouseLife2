const mongoose = require('mongoose');

const gangSchema = new mongoose.Schema({
    name: { type: String, required: true, unique: true },
    ownerCid: { type: String, required: true },
    bank: { type: Number, default: 0 },
    members: [{
        cid: String,
        rank: Number,
    }],
    // You can add more fields here as needed, like territory, etc.
}, { timestamps: true });

module.exports = mongoose.model('Gang', gangSchema);