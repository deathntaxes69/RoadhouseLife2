const Warrant = require('../models/warrantModel');

const cleanWarrant = (warrant) => {
    if (!warrant) return null;
    return {
        m_sId: warrant._id.toString(),
        m_sReason: warrant.reason,
        m_sOfficerCid: warrant.issuing_officer_cid,
        m_sTargetCid: warrant.cid
    };
};

const getWarrants = async (req, res) => {
    try {
        const warrants = await Warrant.find({ cid: req.params.characterId });
        res.status(200).send(JSON.stringify(warrants.map(cleanWarrant)));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const createWarrant = async (req, res) => {
    try {
        const { cid, reason, issuing_officer_cid } = req.body;
        if (!cid || !reason || !issuing_officer_cid) {
            return res.status(400).send(JSON.stringify({ message: 'Missing required fields' }));
        }
        const newWarrant = await Warrant.create({ cid, reason, issuing_officer_cid });
        res.status(201).send(JSON.stringify(cleanWarrant(newWarrant)));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const deleteWarrant = async (req, res) => {
    try {
        const warrant = await Warrant.findByIdAndDelete(req.params.warrantId);
        if (!warrant) {
            return res.status(404).send(JSON.stringify({ message: 'Warrant not found' }));
        }
        res.status(200).send(JSON.stringify({ success: true }));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

module.exports = {
    getWarrants,
    createWarrant,
    deleteWarrant
};