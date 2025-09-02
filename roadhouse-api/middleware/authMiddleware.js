// middleware/authMiddleware.js
const protect = (req, res, next) => {
  const apiKey = req.header('apikey');
  const authHeader = req.header('Authorization');

  let token = '';
  if (apiKey) {
    token = apiKey;
  } else if (authHeader && authHeader.startsWith('Bearer ')) {
    token = authHeader.split(' ')[1];
  }

  if (!token || token !== process.env.API_KEY) {
    return res.status(401).json({ message: 'Not authorized, invalid API key' });
  }

  next(); // If key is valid, proceed to the next middleware/controller
};

module.exports = { protect };