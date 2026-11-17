# Game Rules

## Game Modes

### Star Wars Unlimited

Internal identifier: 0

- PV cannot exceed configured maxPV.
- PV cannot go below zero.
- Refresh resets PV to maxPV.

### Magic: The Gathering

Internal identifier: 1

- PV can exceed configured maxPV.
- maxPV does not restrict increases.
- PV cannot go below zero.
- Refresh resets PV to maxPV.

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
- Set counter to maxPV.
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
against integer overflow. Changing the game retains current totals, as before:
switching from Magic to SWU does not silently clamp a total already above maximum.
Selecting 1/2 players preserves current PV; selecting 3/4 changes nothing and shows
"nao implementado". Saving a maximum restores only the edited player's PV.
Refresh SIM restores all active players (both in 2-player mode), each to their own
maximum. NAO and the 3-second timeout preserve both totals. A hidden player 2
retains PV when switching to 1 player and back during the session. On reboot,
both counters start at their saved maxima; current totals are not persisted.