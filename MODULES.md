# Prototracker-modular modules

Most modules output either in the -1..1 range or similar, this means you can basically connect anything to anything. Experiment.

## Control

### FrequencyIn
| Output | Description |
|--------|-------------|
| 0 | Keydown frequency (1.0 = 440 Hz, 2.0 = 880 Hz etc.)

### TriggerNote 

| Output | Description |
|--------|-------------|
| 0 | Key pressed down |

### Const

Use mousewheel to change the constant value. CTRL/Shift change the speed.

| Output | Description |
|--------|-------------|
| 0 | Dialed value |
| 1 | Dialed value divided by 1000 (useful for milliseconds) |

## Output

### Oscillator

| Input | Description  |
|-------|--------------|
| 0 | Frequency in (rate of which the phase advances) |
| 1 | Sync (rising signal from 0 to 1 sets phase to zero) |
| 2 | Phase add (add to current phase, use for FM) |

| Output | Description  |
|--------|--------------|
| 0 | Sine wave out (-1..1) |
| 1 | Saw wave out (-1..1)  |
