const mongoose = require('mongoose');

const textSchema = new mongoose.Schema({
    senderCid: {
        type: String,
        required: true,
        ref: 'Character'
    },
    receiverCid: {
        type: String,
        required: true,
        ref: 'Character'
    },
    message: {
        type: String,
        required: true
    }
}, {
    timestamps: true
});

const Text = mongoose.model('Text', textSchema);
module.exports = Text;