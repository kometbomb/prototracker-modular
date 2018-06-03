# Prototracker-modular
## in the wilderness

\*A mutant prototracker appeared\*

This is an experimental branch of Prototracker-modular by kometbomb (see the original here: https://github.com/kometbomb/prototracker-modular/releases).

From the original README: "Prototracker-modular is a modular synthesizer fork of Prototracker. The idea is that the user can define his/her "sound chip" using basic modules. Each channel has its own "sound chip". Otherwise, the tracker is exactly like any Prototracker fork.

Double click on synth area to add a new module. Left click to connect inputs/outputs (right click cancels). Drag to move modules. See [MODULES.md](https://github.com/kometbomb/prototracker-modular/blob/master/MODULES.md) for basic info about modules."

## Building

You'll have to figure this one out from build instructions on the main Prototracker-modular branch.


### Building for OSX

To install SDL/SDL2 & SDL2_image on OSX (using brew), type ```brew install sdl2 sdl sdl2_image sdl_mixer sdl_ttf```. You should then be able to run ```make linux```. If you don't want the Mac key binding mods then delete the line -D__MAC_KEYS__=1, or set it to 0.

The binary needs the files in the assets/ directory to work properly.


### Using the Xcode project to build on OSX (10.9+)

This is probably the best way to get it working on MacOS.

It's recommended to use at least Xcode 6.2 with command line tools installed. Requires SDL2 and SDL2_image framework bundles to be installed in default location (/Library/Frameworks).
