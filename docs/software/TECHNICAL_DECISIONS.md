# Technical Decisions

## Platform

- ESP32-S3
- Arduino framework
- C++
- Arduino IDE

## Display

- Library: Arduino_GFX_Library
- Controller: GC9A01A

## Touch

- Controller: CST816S
- Protocol: I2C
- Library: Wire

## Persistence

- Library: Preferences
- Storage: ESP32 NVS

## Timing

Refresh confirmation uses millis().

Avoid blocking delays for application interactions.

## Architecture

The current implementation is a single Arduino sketch.

Consider separating display rendering, touch input,
game rules and persistence into modules as the project grows.

Do not refactor unless requested.