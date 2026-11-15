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