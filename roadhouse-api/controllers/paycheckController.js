// controllers/paycheckController.js
const Paycheck = require('../models/paycheckModel');

const getPaycheck = async (req, res) => {
    try {
        const paycheck = await Paycheck.findOne({ characterId: req.params.characterId });
        if (!paycheck) {
            return res.status(404).json({ message: 'Paycheck not found' });
        }
        res.json(paycheck);
    } catch (error) {
        res.status(500).json({ message: error.message });
    }
};

const createOrUpdatePaycheck = async (req, res) => {
    try {
        const paycheck = await Paycheck.findOneAndUpdate({ characterId: req.body.characterId }, req.body, { new: true, upsert: true });
        res.status(201).json(paycheck);
    } catch (error) {
        res.status(400).json({ message: error.message });
    }
};

module.exports = {
    getPaycheck,
    createOrUpdatePaycheck,
};