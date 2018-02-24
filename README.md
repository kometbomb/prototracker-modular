# Prototracker-modular

Prototracker-modular is a modular synthesizer fork of Prototracker. The idea is that the user can define his/her "sound chip" using basic modules. Each channel has its own "sound chip". Otherwise, the tracker is exactly like any Prototracker fork.

Double click on synth area to add a new module. Left click to connect inputs/outputs (right click cancels). Drag to move modules. See [MODULES.md](https://github.com/kometbomb/prototracker-modular/blob/master/MODULES.md) for basic info about modules.

## Download

See the [releases](https://github.com/kometbomb/prototracker-modular/releases) for binaries.

## Building

Simply do ```make mingw``` where "mingw" is your platform of choice. Do ```make``` to see the supported platforms. 

### Building for OSX

To install SDL/SDL2 on OSX(using brew), type ```brew install sdl2 sdl sdl_image sdl_mixer sdl_ttf portmidi```. If you are getting the ```fatal error: 'SDL_image.h' file not found``` error, type ```apt install libsdl2-image-dev```. You should then be able to run ```make linux```.

The binary needs the files in the assets/ directory to work properly.
