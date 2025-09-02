const Vehicle = require('../models/vehicleModel');

const cleanVehicle = (vehicle) => {
    if (!vehicle) return null;
    return {
        m_sPlate: vehicle.plate,
        m_sPrefab: vehicle.prefab,
        m_iState: vehicle.state || 0,
        m_iGarageId: vehicle.garageId || 0
    };
};

const getVehicles = async (req, res) => {
    try {
        const vehicles = await Vehicle.find({ ownerCid: req.params.cid });
        res.status(200).send(JSON.stringify(vehicles.map(cleanVehicle)));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const addVehicle = async (req, res) => {
    try {
        const { ownerCid, prefab, plate } = req.body;
        await Vehicle.create({ ownerCid, prefab, plate });
        res.status(201).send(JSON.stringify({ success: true }));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

const updateVehicleState = async (req, res) => {
    try {
        const { plate, state, garageId } = req.body;
        await Vehicle.findOneAndUpdate({ plate: plate }, { $set: { state: state, garageId: garageId } });
        res.status(200).send(JSON.stringify({ success: true }));
    } catch (error) {
        res.status(500).send(JSON.stringify({ message: 'Server Error', error: error.message }));
    }
};

module.exports = {
    getVehicles,
    addVehicle,
    updateVehicleState
};