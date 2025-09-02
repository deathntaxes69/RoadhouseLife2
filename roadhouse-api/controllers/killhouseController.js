const KillhouseRecord = require('../models/killhouseRecordModel');

const getKillhouseRecords = async (req, res) => {
    try {
        const records = await KillhouseRecord.find({ mapName: req.params.mapName }).sort({ time: 'asc' }).limit(50);
        res.status(200).send(JSON.stringify(records));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const createKillhouseRecord = async (req, res) => {
    try {
        const { mapName, characterId, characterName, time } = req.body;
        if (!mapName || !characterId || !characterName || !time) {
            return res.status(400).send(JSON.stringify({ message: 'Missing required fields' }));
        }
        const newRecord = await KillhouseRecord.create({ mapName, characterId, characterName, time });
        res.status(201).send(JSON.stringify(newRecord));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

module.exports = {
    getKillhouseRecords,
    createKillhouseRecord
};