# Game Rules

## Game Modes

### Star Wars Unlimited

Internal identifier: 0

- PV cannot exceed configured maxPV.
- PV cannot go below zero.
- Refresh resets PV to zero in SWU and maxPV in Magic.

### Magic: The Gathering

Internal identifier: 1

- PV can exceed configured maxPV.
- maxPV does not restrict increases.
- PV cannot go below zero.
- Refresh resets PV to zero in SWU and maxPV in Magic.

## Maximum PV

- Minimum: 20
- Maximum: 40
- Default: 34

This is a software configuration, not a hardware limitation.

## Refresh Confirmation

When refresh is touched:

1. Display "CERTEZA?".
2. Display SIM and NAO.
3. Wait up to 3 seconds.

SIM:
- Set each active counter to zero in SWU or its maxPV in Magic.
- Return to main screen.

NAO:
- Preserve current PV.
- Return to main screen.

Timeout:
- Preserve current PV.
- Return to main screen automatically.
## Multiple players

Each player uses the existing SWU/Magic rules independently with their own
20..40 maximum (default 34). Game selection remains shared. Increment is guarded
against integer overflow. Changing the game resets both counters (including hidden P2) to zero in SWU or their respective maxima in Magic.
Selecting 1/2 players preserves current PV; selecting 3/4 changes nothing and shows
"nao implementado". Saving a maximum restores only the edited player's PV.
Refresh SIM restores all active players (both in 2-player mode), each to their own
starting value (zero for SWU, maximum for Magic). NAO and the 3-second timeout preserve both totals. A hidden player 2
retains PV when switching to 1 player and back during the session. On reboot,
both counters start at zero in SWU or their saved maxima in Magic; current totals are not persisted.
## Corrected SWU counting (2026-11-17)

SWU tracks accumulated damage from 0 to the configured maximum (inclusive).
Startup, confirmed refresh and saving a maximum reset to 0 in SWU; Magic keeps
starting/resetting at the configured maximum and may increase above it.
Saving a maximum resets only the edited player. Refresh resets active players.
Changing game resets both players to the new game's starting values. Changing
player count preserves totals. Current totals are not persisted; NVS keys are unchanged.
These rules supersede earlier references to always starting/restoring at maximum.

Both games show a small yellow "MAX PV: X" below each life number in one/two-player
layouts, using each player's own maximum. P1's caption rotates with its number.
No GPIO, touch regions, initialization, timing or animation changes are required.

Hardware validation: reboot in SWU and verify 0; tap minus at 0 and plus at the
maximum; confirm refresh returns to 0. Use different maxima (20/40) in two-player
mode and check each caption, P1 orientation, save isolation and refresh of both.
Cancel/timeout must preserve totals. Switch to Magic and verify starting at maximum,
increases above maximum and reset to maximum. Check caption legibility on the LCD.
Compilation alone does not validate physical display/touch behavior.