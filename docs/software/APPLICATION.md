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

## Startup

On startup:
1. Load saved maximum PV.
2. Load saved game mode.
3. Initialize counter using maximum PV.
4. Display main counter screen.

## Counter Behavior

- Right side: increase PV.
- Left side: decrease PV.
- PV cannot go below zero.
- Maximum PV behavior depends on game mode.

See GAME_RULES.md for complete rules.