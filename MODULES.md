# Prototracker-modular modules

Modules are loosely divided in three categories: Control, Generators and Modifiers. This doesn't mean they are limited to that role, though. An oscillator is useful for modifying audio volume as much as an envelope.

Most modules output either in the -1..1 range (or similar), this means you can basically connect anything to anything. Experiment.

Note that there is a delay in signal propagation: every connection between two modules results in a delay of one cycle. This might make things harder with e.g. the accumulator if the reset value is changed at the same time when the reset signal is sent and the signals won't arrive at the exact same cycle.

## Control

These modules are used to control other modules.

### FrequencyIn
| Output | Description |
|--------|-------------|
| 0 | Keydown frequency (1.0 = 440 Hz, 2.0 = 880 Hz etc.)

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

### AudioOut

Use this module to output sound.

| Input | Description  |
|-------|--------------|
| 0 | Mono out |
| 1 | Left channel out |
| 2 | Right channel out |

### EG

Generates a simple attack-decay envelope. Key down input

| Input | Description  |
|-------|--------------|
| 0 | Attack time (in seconds) |
| 1 | Decay time (in seconds) |
| 2 | Key down (>0.5 = down) |

| Output | Description  |
|--------|--------------|
| 0 | Envelope amplitude (0..1) |
| 1 | Envelope finished (pulse 1 when finished, 0 otherwise) |

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

Multiply the two inputs and output the result. Use as an amplifier.

| Input | Description  |
|-------|--------------|
| 0 | Input A |
| 1 | Multiplier |
| 2 | Input B |

| Output | Description  |
|--------|--------------|
| 0 | A multiplied by the multiplier |
| 1 | B multiplied by the multiplier |

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

### Delay

This module delays the signal.

| Input | Description  |
|-------|--------------|
| 0 | Signal in |
| 1 | Delay (in seconds) |
| 2 | Self feedback |

| Output | Description  |
|--------|--------------|
| 0 | Delayed output |
| 1 | Dry output |
