# Agent Instructions

## Project Overview

Embedded life counter running on ESP32-S3 with a 1.28-inch
round LCD touchscreen.

Supported games:
- Star Wars Unlimited (SWU)
- Magic: The Gathering

Development environment:
- Arduino IDE
- C++
- ESP32 Arduino Core

## Documentation

Before modifying the firmware, read the relevant documents.

### Hardware
- docs/hardware/DEVICE.md
- docs/hardware/DISPLAY.md
- docs/hardware/TOUCH.md
- docs/hardware/PINOUT.md

### Software
- docs/software/APPLICATION.md
- docs/software/GAME_RULES.md
- docs/software/UI.md
- docs/software/PERSISTENCE.md
- docs/software/TECHNICAL_DECISIONS.md

## Development Rules

1. Preserve hardware compatibility.
2. Do not change GPIO assignments without justification.
3. Preserve working display and touch initialization.
4. Do not silently change game rules.
5. Keep hardware concerns separate from game logic.
6. Do not introduce animations unless requested.
7. Avoid unnecessary dependencies.
8. Preserve Arduino IDE compatibility.
9. Update documentation when behavior changes.
10. Never claim physical validation based only on compilation.

## Testing

After modifications:
1. Verify compilation when possible.
2. Identify required physical tests.
3. Provide instructions for hardware validation.
4. Clearly distinguish compiled code from physically tested code.

## Code Quality

- Prefer small and understandable changes.
- Preserve existing functionality.
- Explain changes involving GPIO, timing, storage or touch.