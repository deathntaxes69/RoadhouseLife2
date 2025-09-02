const Character = require('../models/characterModel');
const Account = require('../models/accountModel');

const cleanCharacter = (char) => {
    if (!char) return null;
    return {
        uid: char.uid,
        name: char.name,
        age: char.age,
        prefab: char.prefab,
        cash: char.cash,
        bank: char.bank,
        jailTime: char.jailTime,
        licenses: char.licenses || [],
        recipes: char.recipes || [],
        perks: char.perks || [],
        perkPoints: char.perkPoints,
        gangId: char.gangId,
        gangRank: char.gangRank,
        skills: char.skills || {},
        apartmentId: char.apartmentId,
        totalPlayTime: char.totalPlayTime,
        lastLogin: char.lastLogin
    };
};

const fetchCharacters = async (req, res) => {
    try {
        const uid = req.params.uid;
        let account = await Account.findOne({ uid: uid });
        if (!account) {
            const newAccountId = Math.floor(Date.now() / 1000);
            account = await Account.create({ uid, accountId: newAccountId, isWhitelisted: true, isLinked: true });
        }
        const characters = await Character.find({ uid: uid });

        // **THE FIX:** The character array is now nested one level deeper.
         const responseData = {
            data: characters.map(cleanCharacter),
            accountid: account.accountId,
            whitelist: account.isWhitelisted ? 1 : -1,
            linked: account.isLinked ? 1 : -1,
        };
        
        res.status(200).send(JSON.stringify(responseData));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};


const insertCharacter = async (req, res) => {
    try {
        const { name, age, uid, prefab } = req.body;
        if (!name || !age || !uid || !prefab) {
            return res.status(400).json({ message: 'Missing required fields' });
        }
        const newCharacter = await Character.create({ name, age, uid, prefab });
        res.status(201).json(cleanCharacter(newCharacter.toObject()));
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const transactMoney = async (req, res) => {
    try {
        const { cash_update, bank_update } = req.body;
        const character = await Character.findByIdAndUpdate(
            req.params.cid,
            { $inc: { cash: cash_update || 0, bank: bank_update || 0 } },
            { new: true }
        );
        if (!character) return res.status(404).json({ message: 'Character not found' });
        res.status(200).json(cleanCharacter(character));
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const adjustJailTime = async (req, res) => {
    try {
        const { jail_time } = req.body;
        const character = await Character.findByIdAndUpdate(
            req.params.cid,
            { $inc: { jailTime: jail_time || 0 } },
            { new: true }
        );
        if (!character) return res.status(404).json({ message: 'Character not found' });
        res.status(200).json(cleanCharacter(character));
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const fetchLicenses = async (req, res) => {
    try {
        const character = await Character.findById(req.params.characterId).select('licenses');
        res.status(200).json(character ? character.licenses : []);
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const addLicense = async (req, res) => {
    try {
        const { characterId, license, price } = req.params;
        await Character.findByIdAndUpdate(
            characterId,
            { $addToSet: { licenses: license }, $inc: { cash: -parseInt(price) } }
        );
        res.status(200).json({ success: true });
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const removeLicense = async (req, res) => {
    try {
        const { characterId, license } = req.params;
        await Character.findByIdAndUpdate(characterId, { $pull: { licenses: license } });
        res.status(200).json({ success: true });
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const fetchRecipes = async (req, res) => {
    try {
        const character = await Character.findById(req.params.characterId).select('recipes');
        res.status(200).json(character ? character.recipes : []);
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const addRecipe = async (req, res) => {
    try {
        const { characterId, recipe } = req.params;
        await Character.findByIdAndUpdate(characterId, { $addToSet: { recipes: recipe } });
        res.status(200).json({ success: true });
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const removeRecipe = async (req, res) => {
    try {
        const { characterId, recipe } = req.params;
        await Character.findByIdAndUpdate(characterId, { $pull: { recipes: recipe } });
        res.status(200).json({ success: true });
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const setApartmentId = async (req, res) => {
    try {
        const { characterId, apartmentId } = req.params;
        await Character.findByIdAndUpdate(characterId, { apartmentId: parseInt(apartmentId) });
        res.status(200).json({ success: true });
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const updatePlayTime = async (req, res) => {
    try {
        const { characterId, totalPlayTime } = req.params;
        await Character.findByIdAndUpdate(characterId, { totalPlayTime: parseInt(totalPlayTime) });
        res.status(200).json({ success: true });
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const updateLastLogin = async (req, res) => {
    try {
        await Character.findByIdAndUpdate(req.params.characterId, { lastLogin: new Date() });
        res.status(200).json({ success: true });
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

const setGangInfo = async (req, res) => {
    try {
        const { cid, gangId, rank } = req.params;
        await Character.findByIdAndUpdate(cid, { gangId: gangId, gangRank: parseInt(rank) });
        res.status(200).json({ success: true });
    } catch (error) {
        res.status(500).json({ message: 'Server Error', error: error.message });
    }
};

module.exports = {
    fetchCharacters,
    insertCharacter,
    transactMoney,
    adjustJailTime,
    fetchLicenses,
    addLicense,
    removeLicense,
    fetchRecipes,
    addRecipe,
    removeRecipe,
    setApartmentId,
    updatePlayTime,
    updateLastLogin,
    setGangInfo
};