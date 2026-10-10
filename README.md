# SimpleStriped

Demo of [EffectLayer](https://github.com/ygalanter/pebble-effect-layer) for Pebble running the "Mask" effect:
big date, time and day of week with a pattern showing through the digits.

Supports every Pebble platform: aplite, basalt, chalk, diorite, emery, flint and gabbro.
Emery (Pebble Time 2) and gabbro (Pebble Round 2) get larger fonts and 200x220 patterns;
B&W watches (aplite, diorite, flint) get the B&W patterns. The face adapts to Timeline Quick View.

## Building

Requires the [Pebble SDK](https://developer.repebble.com/sdk/) (4.x).

```sh
pebble build
pebble install --emulator emery
```

Settings (background pattern) are provided through [Clay](https://github.com/pebble-dev/clay) and
work offline from the Pebble app.
