const Gang = require('../models/gangModel');
const Character = require('../models/characterModel');

const cleanGang = (gang, members = []) => {
    if (!gang) return null;
    return {
        m_sId: gang._id.toString(),
        m_sGangName: gang.name,
        m_sOwnerCid: gang.ownerCid,
        m_iBank: gang.bank,
        m_aMembers: members
    };
};

const fetchAllGangs = async (req, res) => {
    try {
        const gangs = await Gang.find({});
        res.status(200).send(JSON.stringify(gangs.map(g => cleanGang(g))));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const createGang = async (req, res) => {
    try {
        const { name, ownerCid } = req.body;
        const newGang = await Gang.create({ name, ownerCid, members: [{ cid: ownerCid, rank: 5 }] });
        await Character.findByIdAndUpdate(ownerCid, { gangId: newGang._id.toString(), gangRank: 5 });
        res.status(201).send(JSON.stringify({ m_sId: newGang._id.toString() }));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const gangDeposit = async (req, res) => {
    try {
        const { gangId, amount } = req.params;
        const { cid } = req.body;
        const character = await Character.findById(cid);
        if (!character || character.cash < amount) {
            return res.status(400).send(JSON.stringify({ message: 'Insufficient funds' }));
        }
        character.cash -= parseInt(amount);
        await character.save();
        const gang = await Gang.findByIdAndUpdate(gangId, { $inc: { bank: parseInt(amount) } }, { new: true });
        res.status(200).send(JSON.stringify({ newBalance: gang.bank }));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const gangWithdraw = async (req, res) => {
    try {
        const { gangId, amount } = req.params;
        const { cid } = req.body;
        const gang = await Gang.findById(gangId);
        if (!gang || gang.bank < amount) {
            return res.status(400).send(JSON.stringify({ message: 'Insufficient gang funds' }));
        }
        gang.bank -= parseInt(amount);
        await gang.save();
        await Character.findByIdAndUpdate(cid, { $inc: { cash: parseInt(amount) } });
        res.status(200).send(JSON.stringify({ newBalance: gang.bank }));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

module.exports = {
    fetchAllGangs,
    createGang,
    gangDeposit,
    gangWithdraw
};