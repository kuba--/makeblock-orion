# Makeblock Orion
Arduino Uno scripts for Makeblock Orion board.

Makeblock Orion is a main control board upgraded and improved for teaching and entertainment on the basis of Arduino Uno.

![MeOrion](orion.jpg)

```sh
$ export ARDUINO_DIR=<arduino ide dir>

# Dump build preferences for the current project
$ make dump-prefs

# Compile a particular project with DEBUG flag
$ make project=<project dir> debug=1 compile

# Upload the compiled project
$ make project=<project dir> upload

# Cleanup
$ make project=<project dir> clean

# Upload a pre-compiled hex
$ make hex=<path to hex> upload

# Compile and upload the current project (make all)
$ make
```
