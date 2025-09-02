// controllers/deliveryJobController.js
const DeliveryJob = require('../models/deliveryJobModel');

const getDeliveryJob = async (req, res) => {
    try {
        const job = await DeliveryJob.findById(req.params.id);
        if (!job) {
            return res.status(404).json({ message: 'Delivery job not found' });
        }
        res.json(job);
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
};

const createOrUpdateDeliveryJob = async (req, res) => {
    try {
        const job = await DeliveryJob.findByIdAndUpdate(req.body.id, req.body, { new: true, upsert: true });
        res.status(201).json(job);
    } catch (error) {
        res.status(400).json({ message: error.message });
    }
};

const removeDeliveryJob = async (req, res) => {
    try {
        const job = await DeliveryJob.findByIdAndDelete(req.params.id);
        if (!job) {
            return res.status(404).json({ message: 'Delivery job not found' });
        }
        res.json({ message: 'Delivery job removed' });
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
};

module.exports = {
    getDeliveryJob,
    createOrUpdateDeliveryJob,
    removeDeliveryJob,
};