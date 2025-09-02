const Character = require('../models/characterModel');

const fetchSkills = async (req, res) => {
    try {
        const character = await Character.findById(req.params.characterId).select('skills');
        res.status(200).send(JSON.stringify(character ? character.skills : {}));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const updateSkill = async (req, res) => {
    try {
        const { characterId, skill, value } = req.body;
        const update = {};
        update[`skills.${skill}`] = value;
        
        const character = await Character.findByIdAndUpdate(
            characterId,
            { $set: update }
        );

        if (!character) return res.status(404).send(JSON.stringify({ message: 'Character not found' }));
        res.status(200).send(JSON.stringify({ success: true }));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

module.exports = {
    fetchSkills,
    updateSkill
};