const Text = require('../models/textModel');

const cleanText = (text) => {
    if (!text) return null;
    return {
        m_sId: text._id.toString(),
        m_sSenderCid: text.senderCid,
        m_sReceiverCid: text.receiverCid,
        m_sMessage: text.message
    };
};

const getTexts = async (req, res) => {
    try {
        const characterId = req.params.characterId;
        const texts = await Text.find({
            $or: [{ senderCid: characterId }, { receiverCid: characterId }]
        }).sort({ createdAt: 'asc' });
        res.status(200).send(JSON.stringify(texts.map(cleanText)));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const sendText = async (req, res) => {
    try {
        const { senderCid, receiverCid, message } = req.body;
        if (!senderCid || !receiverCid || !message) {
            return res.status(400).send(JSON.stringify({ message: 'Missing required fields' }));
        }
        const newText = await Text.create({ senderCid, receiverCid, message });
        res.status(201).send(JSON.stringify(cleanText(newText)));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

module.exports = {
    getTexts,
    sendText
};