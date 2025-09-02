const Character = require('../models/characterModel');

const fetchPerks = async (req, res) => {
    try {
        const character = await Character.findById(req.params.characterId).select('perks perkPoints');
        const responseData = {
            perks: character ? character.perks : [],
            perkPoints: character ? character.perkPoints : 0
        };
        res.status(200).send(JSON.stringify(responseData));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const addPerk = async (req, res) => {
    try {
        const { characterId, perk, cost } = req.body;
        await Character.findByIdAndUpdate(
            characterId,
            { $addToSet: { perks: perk }, $inc: { perkPoints: -cost } }
        );
        res.status(200).send(JSON.stringify({ success: true }));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const removePerk = async (req, res) => {
    try {
        const { characterId, perk } = req.body;
        await Character.findByIdAndUpdate(characterId, { $pull: { perks: perk } });
        res.status(200).send(JSON.stringify({ success: true }));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

module.exports = {
    fetchPerks,
    addPerk,
    removePerk
};