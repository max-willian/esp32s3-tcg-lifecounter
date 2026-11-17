# Persistence

## Storage

ESP32 Preferences (NVS).

Namespace: contador

## Stored Settings

| Key | Type | Default |
|---|---|---|
| maxPV | UChar | 34 (player 1, existing key preserved) |
| maxPV2 | UChar | 34 (player 2) |
| jogadores | UChar | 1 |
| jogo | UChar | 0 |

## Game Identifiers

| Value | Game |
|---|---|
| 0 | Star Wars Unlimited |
| 1 | Magic: The Gathering |

## Startup

1. Open Preferences namespace.
2. Read maxPV, maxPV2 and jogadores.
3. Read jogo (shared).
4. Validate values.
5. Initialize both counters at zero in SWU or their respective maximum in Magic.

## Saving

Maximum PV is saved when the user confirms the setting.

Game selection is saved when the user changes game mode.
Saved maxima outside 20..40 fall back to 34 independently. Player counts outside
1..2 fall back to 1. Existing installations keep maxPV/jogo and use defaults for
new keys. Current life totals are not persisted. Maximum PV is saved to maxPV or
maxPV2 according to the edited player; saving resets only that player's PV.
Player count is saved immediately on selecting 1/2. Selecting 3/4 writes nothing.
No namespace, partition, storage type or existing key is changed.
## SWU counting and maximum caption

See GAME_RULES.md for corrected SWU initialization/reset and game-switch rules.
Both layouts show MAX PV: X in 10-pixel smooth text below each number, with
P1's caption rotated. Labels cover the full 20..40 range. Hardware validation
must check zero/maximum boundaries, reset/save isolation, game switching and
caption legibility/orientation. No GPIO, NVS key or touch-region changes.
