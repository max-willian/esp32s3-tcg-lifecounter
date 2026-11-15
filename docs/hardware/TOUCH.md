# Touchscreen

## Controller

CST816S

## Communication

- Protocol: I2C
- Address: 0x15
- SDA: GPIO 11
- SCL: GPIO 7
- RESET: GPIO 6
- Clock: 100 kHz

## Reset Sequence

1. Configure GPIO 6 as OUTPUT.
2. Set GPIO 6 LOW.
3. Wait 50 ms.
4. Set GPIO 6 HIGH.
5. Wait 200 ms.
6. Initialize I2C.

## Coordinate Reading

Read five bytes starting at register 0x02.

Coordinate parsing:

```cpp
x = ((data[1] & 0x0F) << 8) | data[2];
y = ((data[3] & 0x0F) << 8) | data[4];
```

The first byte identifies whether a touch is detected.

## Important

The application detects the beginning of a touch.
Avoid accidentally introducing repeated events while
a finger remains on the screen.