var Clay = require('@rebble/clay');
var clayConfig = require('./config');

// Clay handles showConfiguration / webviewclosed and sends PATTERN to the watch
var clay = new Clay(clayConfig);
