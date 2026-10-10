var Clay = require('@rebble/clay');
var clayConfig = require('./config');
var customClay = require('./custom-clay');

// Clay handles showConfiguration / webviewclosed and sends settings to the watch
var clay = new Clay(clayConfig, customClay);
