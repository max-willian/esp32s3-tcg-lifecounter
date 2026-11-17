# User Interface

## Rendering

240x240 round display, animated hyperspace background on the counter, black
static menus, and vivid yellow (#FFE81F quantized to RGB565 0xFF43) PV/icons/captions.
Menu button labels remain white. Dark neutral surfaces protect icon readability.
No progress ring or MAX caption. Written labels and life digits use Segoe UI Bold
4-bit coverage masks blended into RGB565. Digits select real raster sizes
56/40/28/18/12/10 without enlarging pixels. Labels use 12/16/22-pixel masks.
SmoothTextData.h contains the 30 fixed labels; add new text to GenerateText.ps1.
Regenerate fonts with tools/GenerateText.ps1 and tools/GenerateDigits.ps1 on Windows.
The installed Segoe UI font/System.Drawing are needed only to regenerate assets.
Settings/refresh and circular surfaces use 4-bit masks from GenerateIcons.ps1.
Other icons use UIIcons.h. No runtime font/GIF library or new dependency is needed.
Arduino_Canvas (from the existing Arduino_GFX library) composes a complete frame
before sending it to the LCD. SmoothDigits/Text/Icons blend edge coverage with
the actual canvas pixel, including the animated image and button surfaces.

## Animated background

The user-supplied GIF is converted offline by tools/GenerateBackground.ps1.
See assets/SOURCES.md for provenance and conversion details. The black bars are
cropped away, and the center is resized to 240x240. Navy/gray 4-bit frames have
dimmed highlights to keep yellow PV legible. Twenty frames at 90 ms retain the
original 1.8-second loop with fewer temporal samples (target approximately 11 fps).
Actual LCD frame rate and touch latency require physical validation.

The animation uses millis(), not playback delays. Late frames are skipped with
a bounded catch-up loop. Touch/BOOT and refresh timeout are processed before
animation work. Menus, player selection, maximum editing and confirmation pause
animation; it resumes on returning to the counter. Both player layouts and both
game modes use the animated background. Every redraw restores the current image
before drawing PV, avoiding trails from previous numbers.

One 240x240 RGB565 canvas consumes 115,200 bytes of dynamic heap. If allocation
fails the application logs a warning and keeps working with solid black background
and no animation. The original LCD begin/reset and all GPIO assignments remain.
Completed frames are flushed once per loop when dirty, instead of clearing the
physical LCD before each redraw. Partition/Flash settings and persistence are unchanged.

## One-player counter

The existing layout/touch areas remain. SWU is no longer drawn on the counter;
MAGIC remains visible when selected. Settings at the top, central PV at (120,110), minus left, plus right,
refresh at the bottom. The physical BOOT button also opens settings.
Top menu: x=90..150, y<48. Refresh: x=75..165, y>=165.
Other left/right touches decrement/increment player 1.

## Two-player counter

Horizontal divider at y=120. P1 is the upper half and P2 the lower half.
PV centers are (120,60) and (120,180), using up to 40-pixel-high smooth digits
in a 120-pixel-wide area. Minus/plus appear at x=40/200 in each half.
P1's entire number is rotated 180 degrees to face the opposite player, including
digit order; P2 remains upright. P1's caption is at y=17 above the number in
screen coordinates and is also rotated. P2's caption remains at y=211.
SWU is not drawn on the counter. When selected, MAGIC remains at y=86, rotated
toward P1. Rendering rotation does not rotate the display or touch coordinates;
minus/plus stay in their existing physical quadrants.

Menu/refresh move to the divider, centered at (96,120)/(144,120). Their 40x40
hitboxes (x=76..115 / 124..163, y=100..139) take priority over PV quadrants.
All remaining touches route by y<120 to P1, otherwise P2; x<120 decreases,
x>=120 increases. Each touch changes only its own player. BOOT still opens menu.

## Settings

With one player, page 1 has PV JOGADOR 1, shared game selection and JOGADORES: 1.
With two players, page 1 has PV JOGADOR 1, PV JOGADOR 2 directly below it, then
shared game selection. The > button opens page 2 containing JOGADORES: 2,
a < button to return to page 1, and VOLTAR to the counter.
The second player's settings are inaccessible in 1-player mode.

Maximum-PV editing retains the existing +/- controls, 20..40 range, and SALVAR.
The title identifies the edited player. Saving persists that maximum and restores
only that player's life, then returns to the counter.

## Player selection

Four buttons in a 2x2 grid offer 1/2/3/4. The active 1/2 mode is highlighted.
Selecting 1/2 persists the mode, preserves both current PV totals and returns to
the menu. Selecting 3/4 opens a modal saying "nao implementado" without changing
mode or saving anything; VOLTAR returns to player selection.

## Refresh

CERTEZA?, SIM/NAO and 3-second cancellation remain. For two players, the subtitle
says RESTAURAR AMBOS?. SIM restores all active players to their respective maxima.
NAO and timeout preserve every player's PV.

## Physical button and touch

BOOT (GPIO0, active LOW, INPUT_PULLUP) opens settings only from the counter.
MenuButton.h uses nonblocking 35-ms debounce and one event per press. A held
button does not repeat. A button held at startup is ignored until released.
If touch and BOOT start in the same poll, only the menu opens; the touch must be
released before another action. Touch retains initial-contact detection.
The power button and display/touch GPIO assignments remain unchanged.

## Hardware validation required

- Compile for the existing ESP32-S3 configuration. Compilation is not physical validation.
- Existing installation: confirm saved P1 maximum and game survive; default mode is 1.
- Check the unchanged one-player layout with SWU and MAGIC.
- Select 2, then tap all four quadrants. Confirm only the intended counter changes.
- Hold a finger down: exactly one action per new contact, including screen transitions.
- Check the divider menu/refresh and BOOT do not increment/decrement either PV.
- Navigate both settings pages; P2 is directly below P1 on page 1 only with 2 players.
- From page 2 change player count; verify 1-player menu routing still works.
- Check P1's rotated number from across the table with values 12, 34 and 100
  (correct digit order, no mirroring). Check P1 caption above PV and no SWU label
  on either counter layout; game selection remains available in settings.
- Give P1/P2 different maxima (20 and 40); save each and verify the other PV is preserved.
- Verify each player's zero floor, independent SWU ceiling and Magic above maximum.
- Verify digit fit at 9/10, 99/100, 999/1000 in each sector.
- Select 3 and 4: see nao implementado, unchanged PV/count and working VOLTAR.
- Switch 2 -> 1 -> 2: both PV totals are preserved during the session.
- Refresh SIM restores both maxima in 2-player mode; NAO/3-second timeout preserve both.
- In 1-player mode refresh changes only P1; hidden P2 is preserved.
- Reboot: count/game/maxima persist; each counter starts at its own saved maximum.
- Check text, circular-edge clipping, icon smoothing and touch responsiveness on hardware.
- Check continuous animation in 1/2-player and SWU/Magic modes, yellow PV contrast,
  curved glyph edges over moving bright lines, and absence of trails/flicker.
- Tap each quadrant rapidly and hold touch/BOOT: confirm one action per contact
  and acceptable latency while animating. Check 9/10 and 99/100 for stale pixels.
- Open each menu and refresh confirmation: animation pauses and timeout remains
  3 seconds. Return to the counter: animation resumes without a catch-up burst.
- Inspect serial output for the canvas-allocation warning; a solid background is
  the intentional fallback, not proof of successful animated playback.
- Short-press BOOT only after startup; holding during power-on/reset may enter download mode.
