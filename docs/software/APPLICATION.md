# Application

## Purpose

Life counter for card games running on an ESP32-S3
touchscreen device.

## Supported Games

- Star Wars Unlimited (SWU)
- Magic: The Gathering

## Main Features

- Display current life points.
- Increase and decrease life points.
- Configure maximum life points.
- Select active game.
- Confirm life point refresh.
- Persist settings across reboots.
- Open settings from the counter with either the touch gear or physical BOOT button.
- User-requested animated hyperspace background on the counter and yellow PV/icons.
  Animation pauses in menus and confirmation; controls and game rules are unchanged.

## Startup

On startup:
1. Load saved maximum PV.
2. Load saved game mode.
3. Initialize counter at zero in SWU or maximum PV in Magic.
4. Display main counter screen.

## Counter Behavior

- Right side: increase PV.
- Left side: decrease PV.
- PV cannot go below zero.
- Maximum PV behavior depends on game mode.

See GAME_RULES.md for complete rules.

## Players

The counter supports 1 or 2 players. Options 3/4 show "nao implementado" and
leave the current selection and all settings unchanged. The selected game is
shared, while current PV and maximum PV are independent for each player.
Two-player mode splits at y=120 (P1 above, P2 below). Left/right changes only
that half's PV. Menu/refresh on the divider take priority over quadrant touches.
Switching player count preserves both PV totals within the session. On startup
both counters start at zero in SWU or their respective saved maxima in Magic (default 34).
Saving a maximum restores only the edited player's PV. Confirmed refresh
resets all active players to zero in SWU or their respective maxima in Magic.

## SWU counting and maximum caption

See GAME_RULES.md for corrected SWU initialization/reset and game-switch rules.
Both layouts show MAX PV: X in 10-pixel smooth text below each number, with
P1's caption rotated. Labels cover the full 20..40 range. Hardware validation
must check zero/maximum boundaries, reset/save isolation, game switching and
caption legibility/orientation. No GPIO, NVS key or touch-region changes.
