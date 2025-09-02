const mongoose = require('mongoose');

const accountSchema = mongoose.Schema(
  {
    uid: {
      type: String,
      required: true,
      unique: true,
    },
    accountId: {
      type: Number,
      required: true,
      unique: true,
    },
    isWhitelisted: {
      type: Boolean,
      default: true,
    },
    isLinked: {
      type: Boolean,
      default: true,
    },
  },
  {
    timestamps: true,
  }
);

module.exports = mongoose.model('Account', accountSchema);