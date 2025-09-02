const LapRecord = require('../models/lapRecordModel');

const getLapRecords = async (req, res) => {
    try {
        const records = await LapRecord.find({ trackName: req.params.trackName }).sort({ time: 'asc' }).limit(50);
        res.status(200).send(JSON.stringify(records));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const createLapRecord = async (req, res) => {
    try {
        const { trackName, characterId, characterName, time } = req.body;
        if (!trackName || !characterId || !characterName || !time) {
            return res.status(400).send(JSON.stringify({ message: 'Missing required fields' }));
        }
        const newRecord = await LapRecord.create({ trackName, characterId, characterName, time });
        res.status(201).send(JSON.stringify(newRecord));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

module.exports = {
    getLapRecords,
    createLapRecord
};