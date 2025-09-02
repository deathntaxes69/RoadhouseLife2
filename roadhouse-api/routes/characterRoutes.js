const express = require('express');
const router = express.Router();
const {
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
} = require('../controllers/characterController');

// --- Routes mapped directly from RL_CharacterDbHelper.c ---
router.get('/fetchcharacters/:uid', fetchCharacters);
router.post('/insertcharacter', insertCharacter);
router.post('/transactmoney/:cid', transactMoney);
router.post('/adjustjailtime/:cid', adjustJailTime);
router.get('/fetchlicenses/:characterId', fetchLicenses);
router.post('/addlicense/:characterId/:license/:price', addLicense);
router.post('/removelicense/:characterId/:license', removeLicense);
router.get('/fetchrecipes/:characterId', fetchRecipes);
router.post('/addrecipe/:characterId/:recipe', addRecipe);
router.post('/removerecipe/:characterId/:recipe', removeRecipe);
router.post('/setapartmentid/:characterId/:apartmentId', setApartmentId);
router.get('/updateplaytime/:characterId/:totalPlayTime', updatePlayTime);
router.get('/updatelastlogin/:characterId', updateLastLogin);
router.post('/character/gang/:cid/:gangId/:rank', setGangInfo);

module.exports = router;