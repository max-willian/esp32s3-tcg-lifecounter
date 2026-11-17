# Technical Decisions

## Platform

- ESP32-S3
- Arduino framework
- C++
- Arduino IDE

## Display

- Library: Arduino_GFX_Library
- Controller: GC9A01A

### Smooth typography

Life digits and written labels use Segoe UI Bold masks generated offline on
Windows with System.Drawing. Four-bit pixel coverage is blended in RGB565 using
Arduino_GFX drawPixel. Assets remain in Flash without a runtime font dependency.
Coverage now blends against the current RGB565 canvas pixel to support animation
and existing card/button surfaces without solid-color halos.

SmoothText stores the 30 currently needed labels, totaling
16,934 bytes of mask data. This preserves font kerning without a glyph layout
engine. Add new text to tools/GenerateText.ps1 and regenerate SmoothTextData.h;
unknown strings are not rendered. Existing game rules and touch handling remain
independent of typography.

Settings/refresh icons use 24x24 4-bit masks and 36x36 circular surface masks
(1,224 bytes total) generated offline by tools/GenerateIcons.ps1. No runtime
supersampling is required. The life ring and main MAX caption were
removed; configured maximum PV and its game rules remain unchanged.

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

The GPIO0 BOOT button uses nonblocking 35-ms debounce and generates one event
per press. It opens settings only from the counter so it cannot bypass the
maximum-PV save flow or refresh confirmation. Touch settings access remains.

## Architecture

The current implementation is a single Arduino sketch.

Consider separating display rendering, touch input,
game rules and persistence into modules as the project grows.

Do not refactor unless requested.

## Two-player layout

Counters and maxima use two fixed-size arrays without heap allocation. Each
player has independent current/max PV, with a shared game mode. Both players'
PV settings are grouped on page 1; player-count selection moves to page 2 when
two players are active. Digit masks include a 10-pixel size so every
nonnegative ESP32 int fits the 120-pixel two-player PV area. Normal two-player PV
uses the 40-pixel face. Screen transitions still consume the initial contact;
holding a finger cannot activate an item on the next screen.

SmoothDigits can rotate the entire rendered number 180 degrees inside its own
bounding box, reversing both glyph positions and pixel coordinates. This faces
P1 toward the opposite player without changing LCD rotation, touch mapping,
GPIO or stored values. Rotation itself needs no extra buffer. P1's caption is rotated too.

## Animated hyperspace background

The GIF supplied by the user is converted offline to 20 packed 240x240 4-bit
frames with a fixed dimmed navy/gray palette. Flash data is 576,072 bytes including
palette/timing; no GIF decoder, filesystem, network access, or new dependency is
introduced. The original 60-frame/1.8-second source is kept in assets with provenance.

Arduino_Canvas in the installed Arduino_GFX version provides one 115,200-byte
RGB565 heap buffer. The physical LCD initialization is preserved and canvas.begin
uses GFX_SKIP_OUTPUT_BEGIN to avoid a second display reset. Allocation failure
retains direct display rendering with solid black background and no animation.
Actual heap availability is a hardware/runtime check, not a compiler size result.

The counter decodes packed frames into the canvas, then overlays the existing
widgets; edge alpha samples current canvas pixels. Flush sends the completed
frame once per dirty loop. Animation is scheduled by millis at source durations,
skipping late frames with a maximum 20-step catch-up, and pauses outside the
counter. Hardware tests must check LCD transfer performance, touch responsiveness,
and the unchanged confirmation timeout. Flash size/partition/upload settings and
NVS keys have not been changed. Yellow accent is 0xFF43 (RGB565 #FFE81F).
