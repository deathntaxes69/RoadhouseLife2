// models/characterModel.js
const mongoose = require('mongoose');

const characterSchema = new mongoose.Schema(
  {
    // From insertcharacter
    uid: { type: String, required: true }, // The player's main ID
    name: { type: String, required: true },
    age: { type: Number, required: true },
    prefab: { type: String, required: true },

    // From transactmoney
    cash: { type: Number, default: 0 },
    bank: { type: Number, default: 0 },

    // From adjustjailtime
    jailTime: { type: Number, default: 0 },

    // From license endpoints
    licenses: { type: [String], default: [] },

    // From recipe endpoints
    recipes: { type: [String], default: [] },
    
    // From perk endpoints
    perks: { type: [String], default: [] },
    perkPoints: { type: Number, default: 0 },

    // --- NEWLY ADDED FIELDS ---
    gangId: { type: String, default: "" },
    gangRank: { type: Number, default: 0 },
    skills: { type: Map, of: String, default: {} },
    // -------------------------

    // Other fields
    apartmentId: { type: Number, default: 0 },
    totalPlayTime: { type: Number, default: 0 },
    lastLogin: { type: Date, default: Date.now },
  },
  {
    timestamps: true,
  }
);

// Create an index on uid for faster character lookups
characterSchema.index({ uid: 1 });

module.exports = mongoose.model('Character', characterSchema);