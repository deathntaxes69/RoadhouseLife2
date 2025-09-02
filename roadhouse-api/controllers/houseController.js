const OwnedHouse = require('../models/houseModel');
const { formatForEnfusion } = require('../utils/responseFormatter');

const syncHouses = async (req, res) => {
    try {
        const ownedHouses = await OwnedHouse.find({}).lean();

        // Set the correct Content-Type header for the raw stream format.
        res.setHeader('Content-Type', 'text/plain');

        // Format the response using the special EPF_JSON_STREAM format.
        const formattedResponse = formatForEnfusion(ownedHouses);

        res.status(200).send(formattedResponse);
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const purchaseHouse = async (req, res) => {
    try {
        const { uniqueId, ownerCid, buyPrice } = req.body;
        const newHouse = new OwnedHouse({
            uniqueId,
            ownerCid,
            buyPrice
        });
        await newHouse.save();
        res.status(201).json({ success: true, house: newHouse });
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const sellHouse = async (req, res) => {
    try {
        const { uniqueId } = req.params;
        await OwnedHouse.deleteOne({ uniqueId });
        res.status(200).json({ success: true });
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const addUpgrade = async (req, res) => {
    try {
        const { uniqueId } = req.params;
        const { upgradeType, upgradeName } = req.body;

        const house = await OwnedHouse.findOne({ uniqueId: uniqueId });
        if (!house) {
            return res.status(404).json({ message: 'House not found' });
        }

        house.upgrades.push({ type: upgradeType, name: upgradeName });
        await house.save();

        res.status(200).json({ success: true, upgrades: house.upgrades });
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const addDynamicUpgrade = async (req, res) => {
    try {
        const { uniqueId } = req.params;
        const { upgradeUid, prefabPath, position, rotation } = req.body;

        const house = await OwnedHouse.findOne({ uniqueId: uniqueId });
        if (!house) {
            return res.status(404).json({ message: 'House not found' });
        }

        house.dynamicUpgrades.push({ upgradeUid, prefabPath, position, rotation });
        await house.save();

        res.status(200).json({ success: true, dynamicUpgrades: house.dynamicUpgrades });
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const removeDynamicUpgrade = async (req, res) => {
    try {
        const { upgradeUid } = req.params;
        
        await OwnedHouse.updateOne(
            { "dynamicUpgrades.upgradeUid": upgradeUid },
            { $pull: { dynamicUpgrades: { upgradeUid: upgradeUid } } }
        );

        res.status(200).json({ success: true });
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};


module.exports = {
    syncHouses,
    purchaseHouse,
    sellHouse,
    addUpgrade,
    addDynamicUpgrade,
    removeDynamicUpgrade
};