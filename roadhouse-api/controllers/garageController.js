const GarageVehicle = require('../models/garageVehicleModel');

const cleanGarageVehicle = (vehicle) => {
    if (!vehicle) return null;
    return {
        m_sVehicleId: vehicle.vehicleId,
        m_sPrefab: vehicle.prefab,
        m_iState: vehicle.state || 0, // Assuming 0 is a default state
        m_sDisplayName: vehicle.displayName || ""
    };
};

const getGarageVehicles = async (req, res) => {
    try {
        const vehicles = await GarageVehicle.find({ characterId: req.params.characterId });
        res.status(200).send(JSON.stringify(vehicles.map(cleanGarageVehicle)));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const addVehicleToGarage = async (req, res) => {
    try {
        const { characterId, vehicleId, prefab, state, mods } = req.body;
        if (!characterId || !vehicleId || !prefab) {
            return res.status(400).send(JSON.stringify({ message: 'Missing required fields' }));
        }
        const newVehicle = await GarageVehicle.create({ characterId, vehicleId, prefab, state, mods });
        res.status(201).send(JSON.stringify(cleanGarageVehicle(newVehicle)));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const updateVehicleState = async (req, res) => {
    try {
        const { vehicleId, state, mods } = req.body;
        if (!vehicleId || !state) {
            return res.status(400).send(JSON.stringify({ message: 'Missing required fields' }));
        }
        const updatedVehicle = await GarageVehicle.findOneAndUpdate(
            { vehicleId: vehicleId },
            { state: state, mods: mods },
            { new: true }
        );
        if (!updatedVehicle) {
            return res.status(404).send(JSON.stringify({ message: 'Vehicle not found in garage' }));
        }
        res.status(200).send(JSON.stringify(cleanGarageVehicle(updatedVehicle)));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

module.exports = {
    getGarageVehicles,
    addVehicleToGarage,
    updateVehicleState
};