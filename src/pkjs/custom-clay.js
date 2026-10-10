// Runs inside the generated config page (injected via toString), so it must be self-contained
module.exports = function(minified) {
  var clayConfig = this;
  var SOLID_COLOR = '20';

  function toggleColorPickers() {
    var solid = this.get() === SOLID_COLOR;
    ['TEXT_COLOR', 'BG_COLOR'].forEach(function(key) {
      var item = clayConfig.getItemByMessageKey(key);
      if (solid) {
        item.show();
      } else {
        item.hide();
      }
    });
  }

  clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
    var pattern = clayConfig.getItemByMessageKey('PATTERN');
    toggleColorPickers.call(pattern);
    pattern.on('change', toggleColorPickers);
  });
};
