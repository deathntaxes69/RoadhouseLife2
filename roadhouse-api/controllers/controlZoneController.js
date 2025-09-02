const ControlZone = require('../models/controlZoneModel');

const cleanZone = (zone) => {
    if (!zone) return null;
    return {
        m_iZoneId: zone.zoneId,
        m_sOwnerFaction: zone.ownerFaction || "",
        m_iTaxAmount: zone.taxAmount || 0
    };
};

const getZone = async (req, res) => {
    try {
        const zoneId = parseInt(req.params.id);
        let zone = await ControlZone.findOne({ zoneId: zoneId });
        if (!zone) {
            zone = await ControlZone.create({ zoneId: zoneId });
        }
        res.status(200).send(JSON.stringify(cleanZone(zone)));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const setOwner = async (req, res) => {
    try {
        const { id, faction } = req.params;
        await ControlZone.findOneAndUpdate(
            { zoneId: parseInt(id) },
            { $set: { ownerFaction: faction } },
            { upsert: true }
        );
        res.status(200).send(JSON.stringify({ success: true }));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const setTax = async (req, res) => {
    try {
        const { id, amount } = req.params;
        await ControlZone.findOneAndUpdate(
            { zoneId: parseInt(id) },
            { $set: { taxAmount: parseInt(amount) } },
            { upsert: true }
        );
        res.status(200).send(JSON.stringify({ success: true }));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

module.exports = {
    getZone,
    setOwner,
    setTax
};