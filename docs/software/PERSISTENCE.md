# Persistence

## Storage

ESP32 Preferences (NVS).

Namespace: contador

## Stored Settings

| Key | Type | Default |
|---|---|---|
| maxPV | UChar | 34 |
| jogo | UChar | 0 |

## Game Identifiers

| Value | Game |
|---|---|
| 0 | Star Wars Unlimited |
| 1 | Magic: The Gathering |

## Startup

1. Open Preferences namespace.
2. Read maxPV.
3. Read jogo.
4. Validate values.
5. Initialize counter to maxPV.

## Saving

Maximum PV is saved when the user confirms the setting.

Game selection is saved when the user changes game mode.