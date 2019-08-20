# Prototracker-modular modules

Modules are loosely divided in three categories: Control, Generators and Modifiers. This doesn't mean they are limited to that role, though. An oscillator is useful for modifying audio volume as much as an envelope.

- [Prototracker-modular modules](#prototracker-modular-modules)
  - [Good to know](#good-to-know)
  - [Control](#control)
    - [FrequencyIn](#frequencyin)
    - [Volume](#volume)
    - [TriggerNote](#triggernote)
    - [Const](#const)
    - [Effect](#effect)
    - [Automation](#automation)
    - [AudioOut](#audioout)
    - [Oscilloscope](#oscilloscope)
    - [VUMeter](#vumeter)
    - [EG](#eg)
    - [Accumulator](#accumulator)
    - [Semitone](#semitone)
  - [Generators](#generators)
    - [Oscillator](#oscillator)
    - [Pulse](#pulse)
    - [Noise](#noise)
    - [Sample](#sample)
  - [Modifiers](#modifiers)
    - [Add](#add)
    - [Mul](#mul)
    - [Div](#div)
    - [Abs](#abs)
    - [Bits](#bits)
    - [Shape](#shape)
    - [Linear](#linear)
    - [Clamp](#clamp)
    - [Distorion](#distorion)
    - [Split](#split)
    - [RMS](#rms)
    - [Mixer](#mixer)
    - [Lerp](#lerp)
    - [Filter](#filter)
    - [Glide](#glide)
    - [Delay](#delay)
    - [Reverb](#reverb)
    - [Container](#container)
    - [ExtIn/ExtOut](#extinextout)
    - [Virtual](#virtual)

## Good to know

Most modules output either in the -1..1 range (or similar), this means you can basically connect anything to anything. Experiment.

Note that there is a delay in signal propagation: every connection between two modules results in a delay of one cycle. This might make things harder with e.g. the accumulator if the reset value is changed at the same time when the reset signal is sent and the signals won't arrive at the exact same cycle.

## Control

These modules are used to control other modules.

### FrequencyIn

| Output | Description |
|--------|-------------|
| 0 | Keydown frequency in kHz |

### Volume

| Output | Description |
|--------|-------------|
| 0 | Current track volume (0..1.0) |

### TriggerNote

Note that this is just the initial keypress. You need to implement key-off functionality yourself (e.g. with the Effect module).

| Output | Description |
|--------|-------------|
| 0 | Key pressed down (0 = not pressed, 1.0 = pressed down) |

### Const

Use mousewheel to change the constant value. CTRL/Shift change the speed.

| Output | Description |
|--------|-------------|
| 0 | Dialed value |
| 1 | Dialed value divided by 1000 (useful for milliseconds) |

### Effect

This outputs whatever value was used as a parameter for a pattern effect. Use mousewheel to select the effect. E.g. in pattern you have X40, module now outputs 1.0.

| Output | Description |
|--------|-------------|
| 0 | Effect value (0x00 = 0, 0x40 = 1.0, 0x80 = 2.0 etc.) |
| 1 | Effect value (0x00 = 0, 0x40 = 64, 0x80 = 128 etc.) |

### Automation

This module outputs the value from the automation timeline at the current song position. Use mousewheel to select the automation track.

| Output | Description |
|--------|-------------|
| 0 | Automation value (0.0..1.0) |

### AudioOut

Use this module to output sound.

| Input | Description  |
|-------|--------------|
| 0 | Mono out |
| 1 | Left channel out |
| 2 | Right channel out |

### Oscilloscope

Use this to examine a signal. Put between two modules and let the signal pass thru.

This module has two modes: auto sync and manual sync. Auto mode tries to display a single cycle of the signal and
the manual mode can be triggered with the sync signal. E.g. use a saw wave signal at 50 Hz to trigger every 1/50th
of a second or instead of a fixed frequency use FrequencyIn to sync to the played note.

| Input | Description  |
|-------|--------------|
| 0 | Manual sync input |
| 1 | Auto sync input |
| 2 | Sync (rising signal crossing 0.5 triggers an update) |

| Output | Description  |
|-------|--------------|
| 0 | Input 0 + Input 1 |

### VUMeter

Use this to examine a signal. Put between two modules and let the signal pass thru.

This module has two modes (selectable with the mousewheel): one to visualize values between 0 and 1 and another
to visualize values between -1 and 1.

| Input | Description  |
|-------|--------------|
| 0 | Input |

| Output | Description  |
|-------|--------------|
| 0 | Passthru |

### EG

Generates a simple attack-decay envelope. The envelope output will rise from 0.0 to 1.0 and stay at 1.0 until key down input is released.
The hard reset input does the same but also sets the envelope volume to zero when triggered but it will NOT keep the envelope at full
output (the decay state will automatically start after the envelope reaches 1.0).

| Input | Description  |
|-------|--------------|
| 0 | Attack time (in seconds) |
| 1 | Decay time (in seconds) |
| 2 | Key down (>0.5 = down) |
| 3 | Key down with hard reset |

| Output | Description  |
|--------|--------------|
| 0 | Envelope amplitude (0..1) |
| 1 | Envelope finished (pulse 1 when finished, 0 otherwise) |
| 2 | Envelope state (0 = attack, 1 = decay) |

### Accumulator

This module simply accumulates an internal register. Useful as some kind of rudimentary envelope.

| Input | Description  |
|-------|--------------|
| 0 | Add to accumulator |
| 1 | Reset (rising signal crossing 0.5 triggers reset to the amplitude in input 2) |
| 2 | Reset value |

| Output | Description  |
|--------|--------------|
| 0 | Accumulator |

### Semitone

Use this module with Mul to transpose frequency. Especially useful with Effect.

| Input | Description  |
|-------|--------------|
| 0 | Relative semitones |

| Output | Description  |
|--------|--------------|
| 0 | Ratio of transpose (input of 0 semitones = 1.0, input +12 semis = 2.0) |

## Generators

These modules are used to generate signals useful as audio.

### Oscillator

| Input | Description  |
|-------|--------------|
| 0 | Frequency in (rate at which the phase advances, in kHz) |
| 1 | Sync (rising signal crossing 0.5 sets phase to zero) |
| 2 | Phase add (add to current phase, use for FM) |

| Output | Description  |
|--------|--------------|
| 0 | Sine wave out (-1..1) |
| 1 | Saw wave out (-1..1)  |

### Pulse

| Input | Description  |
|-------|--------------|
| 0 | Frequency in (rate at which the phase advances, in kHz) |
| 1 | Sync (rising signal crossing 0.5 sets phase to zero) |
| 2 | Pulse width (-1..1) |

| Output | Description  |
|--------|--------------|
| 0 | Pulse wave out (-1..1) |

### Noise

| Input | Description  |
|-------|--------------|
| 0 | Frequency in (rate at which the phase advances) |
| 1 | Sync (rising signal crossing 0.5 sets phase to zero) |

| Output | Description  |
|--------|--------------|
| 0 | Noise out (-1..1) |
| 1 | 4-bit noise out (-1..1) |
| 2 | 1-bit noise out (-1..1) |

### Sample

This module will output the buffer amplitude at the position (0..1) specified by input 0. Double click to open a file dialog to open a WAV file, up to two channels are supported.

An easy way to play a sample at its base rate (as specified by the WAV file) is to hook output #3 to an EG module ATTACK input and the EG output #0 back to the sample input #0. When the EG module is triggered the sample module will output the sample amplitude at the correct speed.

| Input | Description  |
|-------|--------------|
| 0 | Sample position (0..1) |

| Output | Description  |
|--------|--------------|
| 0 | Mono out (-1..1) |
| 1 | Left channel out (-1..1) |
| 2 | Right channel out (-1..1) |
| 3 | Sample length in seconds |

## Modifiers

These modules are used to combine and modify signals.

### Add

Add inputs together and output the result.

| Input | Description  |
|-------|--------------|
| 0 | Input A |
| 1 | Input B |
| 2 | Input C |

| Output | Description  |
|--------|--------------|
| 0 | The sum of inputs |

### Mul

Multiply the two inputs and output the result. Use as an amplifier. Connect e.g. an EG to input 1
and use it to multiple the two separate inputs to synchronize.

| Input | Description  |
|-------|--------------|
| 0 | Input A |
| 1 | Multiplier |
| 2 | Input B |

| Output | Description  |
|--------|--------------|
| 0 | A multiplied by the multiplier |
| 1 | B multiplied by the multiplier |

### Div

Divide the first input by the second output the result.

| Input | Description  |
|-------|--------------|
| 0 | Input |
| 1 | Divisor |

| Output | Description  |
|--------|--------------|
| 0 | Input divided by the divisor |

### Abs

Outputs the absolute value of the input.

| Input | Description  |
|-------|--------------|
| 0 | Input |

| Output | Description  |
|--------|--------------|
| 0 | Absolute value of input |

### Bits

Converts the input to any bit depth between 1..16.

| Input | Description  |
|-------|--------------|
| 0 | Input |
| 1 | Bit depth |

| Output | Description  |
|--------|--------------|
| 0 | Bit reduced input value |

### Shape

Shape outputs input #1 to the power of input #2. Useful as a distortion and to shape envelopes.

| Input | Description  |
|-------|--------------|
| 0 | Input |
| 1 | Shape |

| Output | Description  |
|--------|--------------|
| 0 | Shaped output |

### Linear

This output shapes an input so that the linearity has a kink at (X,Y). Use for phase distortion
synthesis by shaping a saw wave and feeding into the oscillator phase input.

| Input | Description  |
|-------|--------------|
| 0 | Input |
| 1 | Kink X position |
| 2 | Kink Y position |

| Output | Description  |
|--------|--------------|
| 0 | Shaped output |

### Clamp

Clamps the input to set range.

| Input | Description  |
|-------|--------------|
| 0 | Input |
| 1 | Lower limit |
| 2 | Upper limit |

| Output | Description  |
|--------|--------------|
| 0 | Clamped output |

### Distorion

Soft clipping.

| Input | Description  |
|-------|--------------|
| 0 | Input |
| 1 | Distortion |

| Output | Description  |
|--------|--------------|
| 0 | Soft clipped output |

### Split

Use this to split signals.

| Input | Description  |
|-------|--------------|
| 0 | Input |

| Output | Description  |
|--------|--------------|
| 0 | Same as input |
| 1 | Same as input |
| 2 | Same as input |

### RMS

This module outputs the root mean square of the input (window size 20 milliseconds). This results in the amplitude of the input.

| Input | Description  |
|-------|--------------|
| 0 | Input |

| Output | Description  |
|--------|--------------|
| 0 | Amplitude (root mean square) |

### Mixer

Splits input A two outputs based on input B. In other words, use this to create stereo panning (or, use for other purposes where you need a conditionally split signals).

| Input | Description  |
|-------|--------------|
| 0 | Input |
| 1 | Mix (-1.0 = only output to output A, 0.0 = output to both outputs, 1.0 = only output to output B) |

| Output | Description  |
|--------|--------------|
| 0 | Output A |
| 1 | Output B |

### Lerp

This module takes in two inputs and interpolates (linearly) between the two inputs.

| Input | Description  |
|-------|--------------|
| 0 | Input A |
| 1 | Mix (0 = output A, 1.0 = output B) |
| 2 | Input B |

| Output | Description  |
|--------|--------------|
| 0 | Output |

### Filter

| Input | Description  |
|-------|--------------|
| 0 | Signal in |
| 1 | Cutoff frequency (in kHz) |
| 2 | Resonance (0..1.0) |

| Output | Description  |
|--------|--------------|
| 0 | Low-pass output |
| 1 | High-pass output |

### Glide

This module will follow the input signal at a constant speed.

| Input | Description  |
|-------|--------------|
| 0 | Signal in |
| 1 | Glide speed |

| Output | Description  |
|--------|--------------|
| 0 | Output |

### Delay

This module delays the input signal. Optionally, the signal can be fed back in the
delay buffer via input #3 (connect from output 1 to input 3) which gets amplified
by input #2. Input gain controls the delay amplitude and also the feedback ratio
if the signal is fed back.

| Input | Description  |
|-------|--------------|
| 0 | Signal in |
| 1 | Delay (in seconds) |
| 2 | Input gain |
| 3 | Feedback input |

| Output | Description  |
|--------|--------------|
| 0 | Combined output |
| 1 | Wet output |

### Reverb

Adds reverberation to input signal. Reverb time is the feedback gain inside
the reverb, so it should be set -0.999..0.999 to avoid distortion.

Input #2 can be used to adjust the delay loop lengths which is useful when
creating stereo reverb effects.

| Input | Description  |
|-------|--------------|
| 0 | Signal in |
| 1 | Reverb time |
| 2 | Delay offset |

| Output | Description  |
|--------|--------------|
| 0 | Combined output |
| 1 | Wet output |

### Container

This can be used to host another whole synth inside a synth. The inputs
and outputs depend on the contained input/output modules.

Double click the module to access its insides. You can use CTRL+O and
CTRL+S to load and save whatever is inside the module. To use partials,
first create a Container, double-click and load a new synth partial with
CTRL+O.

### ExtIn/ExtOut

These modules act as the interface between the Container module and the
host synth. Looking from outside the Container, these show up as normal
module inputs and outputs on the Container.

Use the mouse wheel to select which input or output the modules connect to.

### Virtual

This module can be used to host multiple polyphonic instances of the same layout.
The difference between a Container and a Virtual module is that Virtual
only has left/right/mono audio outputs and the hosted layout needs to use the
AudioOut module for any outputs. ExtIn modules can be used as with Container.

Whenever the AudioOut signal is close to zero for some duration the track
is stopped and queued as the next available track. This means all layouts
should e.g. fade out when TriggerNote is not outputting a signal.

The first input is required and it can be routed from e.g. TriggerNote.
Whenever the first input is triggered a new virtual track is picked from the
available tracks and the ExtIn signals are routed to the new track.

| Input | Description  |
|-------|--------------|
| 0 | KeyOn |
| 1.. | ExtIn 0.. |

| Output | Description  |
|--------|--------------|
| 0 | Mono out |
| 1 | Left out |
| 2 | Right out |
