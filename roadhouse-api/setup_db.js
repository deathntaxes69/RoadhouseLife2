const mongoose = require('mongoose');
const dotenv = require('dotenv');
const connectDB = require('./config/db');

// Load all models to ensure all collections are targeted
const Character = require('./models/characterModel');
const ControlZone = require('./models/controlZoneModel');
const Gang = require('./models/gangModel');
const House = require('./models/houseModel');
const Vehicle = require('./models/vehicleModel');
const CrimeHistory = require('./models/crimeHistoryModel');
const PlayerLog = require('./models/playerLogModel');
const Warrant = require('./models/warrantModel');
const GarageVehicle = require('./models/garageVehicleModel');
const Text = require('./models/textModel');
const KillhouseRecord = require('./models/killhouseRecordModel');
const LapRecord = require('./models/lapRecordModel');

dotenv.config();

const setupDatabase = async () => {
    try {
        await connectDB();

        // --- Clear All Data From All Collections ---
        console.log('--- Wiping all collections from the database ---');

        await Character.deleteMany({});
        console.log('✔ Characters cleared.');

        await ControlZone.deleteMany({});
        console.log('✔ Control Zones cleared.');

        await Gang.deleteMany({});
        console.log('✔ Gangs cleared.');

        await House.deleteMany({});
        console.log('✔ Houses cleared.');

        await Vehicle.deleteMany({});
        console.log('✔ Vehicles cleared.');

        await CrimeHistory.deleteMany({});
        console.log('✔ Crime History cleared.');

        await PlayerLog.deleteMany({});
        console.log('✔ Player Logs cleared.');

        await Warrant.deleteMany({});
        console.log('✔ Warrants cleared.');

        await GarageVehicle.deleteMany({});
        console.log('✔ Garage Vehicles cleared.');

        await Text.deleteMany({});
        console.log('✔ Texts cleared.');

        await KillhouseRecord.deleteMany({});
        console.log('✔ Killhouse Records cleared.');

        await LapRecord.deleteMany({});
        console.log('✔ Lap Records cleared.');

        console.log('\n--- Database is now empty and ready. ---');

    } catch (error) {
        console.error('An error occurred during database setup:', error);
    } finally {
        mongoose.connection.close();
        console.log('Database connection closed.');
    }
};

setupDatabase();