// models/deliveryJobModel.js
const mongoose = require('mongoose');

const deliveryJobSchema = new mongoose.Schema({
    id: { type: String, required: true, unique: true },
    jobName: { type: String, required: true },
    destination: { type: String, required: true },
    reward: { type: Number, required: true },
    isCompleted: { type: Boolean, default: false }
});

const DeliveryJob = mongoose.model('DeliveryJob', deliveryJobSchema);

module.exports = DeliveryJob;