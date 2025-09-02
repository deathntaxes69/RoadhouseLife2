const House = require('../models/houseModel');

const syncHouses = async (req, res) => {
    try {
        const { houses } = req.body;
        if (!Array.isArray(houses)) {
            return res.status(400).json({ message: 'Invalid request body: houses must be an array.' });
        }

        const operations = houses.map(house => ({
            updateOne: {
                filter: { uniqueId: house.uniqueId },
                update: { $set: house },
                upsert: true,
                setDefaultsOnInsert: true
            }
        }));

        if (operations.length > 0) {
            await House.bulkWrite(operations);
        }
        
        const ownedHouses = await House.find({ ownerCid: { $ne: null } });
        
        const responseData = ownedHouses.map(h => ({
            uniqueId: h.uniqueId,
            ownerCid: h.ownerCid,
            upgrades: h.upgrades || [],
            dynamicUpgrades: []
        }));

        res.status(200).json(responseData);
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const getHouses = async (req, res) => {
    try {
        const houses = await House.find({});
        res.status(200).json(houses);
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const updateHouse = async (req, res) => {
    try {
        const { ownerCid, isLocked } = req.body;
        const house = await House.findByIdAndUpdate(req.params.id, { ownerCid, isLocked }, { new: true });
        if (!house) return res.status(404).json({ message: 'House not found' });
        res.status(200).json(house);
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const addUpgrade = async (req, res) => {
    try {
        const { upgradeId } = req.body;
        if (!upgradeId) return res.status(400).json({ message: 'Upgrade ID is required' });
        const house = await House.findByIdAndUpdate(req.params.id, { $addToSet: { upgrades: upgradeId } }, { new: true });
        if (!house) return res.status(404).json({ message: 'House not found' });
        res.status(200).json(house.upgrades);
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const removeUpgrade = async (req, res) => {
    try {
        const { upgradeId } = req.params;
        const house = await House.findByIdAndUpdate(req.params.id, { $pull: { upgrades: upgradeId } }, { new: true });
        if (!house) return res.status(404).json({ message: 'House not found' });
        res.status(200).json({ success: true });
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const createHouse = async (req, res) => {
    try {
        const house = new House(req.body);
        await house.save();
        res.status(201).json(house);
    } catch (error) {
        res.status(400).json({ message: 'Error creating house', error: error.message });
    }
};

module.exports = {
    syncHouses,
    getHouses,
    updateHouse,
    createHouse,
    addUpgrade,
    removeUpgrade
};