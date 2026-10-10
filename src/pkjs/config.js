// Clay configuration page: https://github.com/pebble-dev/clay
var bwPatterns = [
  { label: 'Horizontal Stripes', value: '0' },
  { label: 'Vertical Stripes', value: '1' },
  { label: 'TV Noise', value: '2' },
  { label: 'Grid', value: '3' },
  { label: 'Solid Color', value: '20' }
];

var colorPatterns = [
  { label: 'Horizontal Stripes', value: '0' },
  { label: 'Horizontal Color Stripes', value: '10' },
  { label: 'Vertical Stripes', value: '1' },
  { label: 'Vertical Color Stripes', value: '11' },
  { label: 'TV Noise', value: '2' },
  { label: 'TV Noise Color', value: '12' },
  { label: 'Grid', value: '3' },
  { label: 'Sea Sunset', value: '13' },
  { label: 'Solid Color', value: '20' }
];

module.exports = [
  {
    type: 'heading',
    defaultValue: 'Simple Striped'
  },
  {
    type: 'section',
    items: [
      {
        type: 'heading',
        defaultValue: 'Background pattern'
      },
      {
        type: 'radiogroup',
        messageKey: 'PATTERN',
        defaultValue: '0',
        capabilities: ['BW'],
        options: bwPatterns
      },
      {
        type: 'radiogroup',
        messageKey: 'PATTERN',
        defaultValue: '0',
        capabilities: ['COLOR'],
        options: colorPatterns
      },
      // color pickers are shown only when "Solid Color" is selected (see custom-clay.js).
      // Clay limits them to black & white on B&W watches
      {
        type: 'color',
        messageKey: 'TEXT_COLOR',
        label: 'Time and date color',
        defaultValue: '0xFFFFFF'
      },
      {
        type: 'color',
        messageKey: 'BG_COLOR',
        label: 'Screen color',
        defaultValue: '0x000000'
      }
    ]
  },
  {
    type: 'submit',
    defaultValue: 'Save'
  }
];
