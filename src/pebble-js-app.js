var version = 1.81;

Pebble.addEventListener("ready",
  function(e) {
    // console.log("PebbleKit JS ready!");
  }
);



Pebble.addEventListener("showConfiguration",
  function(e) {
    
    //getting platform
    var platform;
    if(Pebble.getActiveWatchInfo) {
      // Available for use!
      try {
         platform = Pebble.getActiveWatchInfo().platform;
      } catch(err) {
         platform = "basalt";
      }
    } else {
      // Not available, handle gracefully
      platform = 'aplite';
    }
   
    //Load the remote config page
    Pebble.openURL("http://codecorner.galanter.net/pebble/simple_striped/simple_striped_config.html?version=" + version + "&platform=" + platform);
    
  }
);

Pebble.addEventListener("webviewclosed",
  function(e) {
    
    if (e.response !== '') {
      
      console.log('resonse: ' + decodeURIComponent(e.response));
      
      //Get JSON dictionary
      var settings = JSON.parse(decodeURIComponent(e.response));
      
      console.log(settings);
      
      //Send to Pebble, persist there
      Pebble.sendAppMessage(
          {
            "KEY_PATTERN": settings.pattern
          },
        function(e) {
          console.log("Sending settings data...");
        },
        function(e) {
          console.log("Settings feedback failed!");
        }
      );
    }
  }
);