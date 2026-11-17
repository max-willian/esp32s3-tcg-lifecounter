# Hyperspace background

User-supplied source, downloaded 2026-11-17:
https://cdn.dribbble.com/userupload/42195841/file/original-5805f31455ae22d5a1f005704222ee97.gif

The original GIF is retained as hyperspace.gif (800x600, 60 frames, 1.8 seconds).
No ownership or redistribution license is asserted here.

tools/GenerateBackground.ps1 crops the center 456x456 region at (172,72) to
remove the black letterbox bars, resizes to 240x240, samples every third frame,
and converts to a 16-level navy/gray palette with dimmed highlights.
The generated HyperspaceData.cpp/.h contains 20 packed 4-bit frames (576,000
bytes), palette (32 bytes) and durations (40 bytes). Each frame lasts 90 ms;
the loop retains the original 1,800-ms duration, with reduced temporal detail.

assets/background-preview.png is the converted first frame. The original GIF
is an offline build input only; the ESP32 does not fetch the URL or decode GIFs.
