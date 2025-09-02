const PlayerLog = require('../models/playerLogModel');
const CrimeHistory = require('../models/crimeHistoryModel');

const createPlayerLog = async (req, res) => {
    try {
        const { log_type, log_data } = req.body;
        await PlayerLog.create({
            characterId: req.params.characterId,
            logType: log_type,
            logData: log_data,
        });
        res.status(201).send(JSON.stringify({ success: true }));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const createCrimeHistoryLog = async (req, res) => {
    try {
        const { cid, charge, ticket_amount, jail_time_amount, officer_charging_cid } = req.body;
        await CrimeHistory.create({
            characterId: cid,
            charge: charge,
            ticketAmount: ticket_amount,
            jailTimeAmount: jail_time_amount,
            officerChargingId: officer_charging_cid,
        });
        res.status(201).send(JSON.stringify({ success: true }));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

module.exports = {
    createPlayerLog,
    createCrimeHistoryLog
};