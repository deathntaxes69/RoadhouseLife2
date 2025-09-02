const mongoose = require('mongoose');

const warrantSchema = new mongoose.Schema({
    cid: {
        type: String,
        required: true,
        ref: 'Character'
    },
    reason: {
        type: String,
        required: true
    },
    issuing_officer_cid: {
        type: String,
        required: true,
        ref: 'Character'
    }
}, {
    timestamps: true
});

const Warrant = mongoose.model('Warrant', warrantSchema);
module.exports = Warrant;