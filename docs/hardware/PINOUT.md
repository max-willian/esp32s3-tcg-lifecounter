# Hardware Pinout

## LCD - GC9A01A

| Signal | ESP32-S3 GPIO |
|---|---:|
| MOSI | 2 |
| SCLK | 4 |
| CS | 5 |
| DC | 47 |
| RST | 38 |
| Backlight | 42 |

Backlight:
- LOW: ON
- HIGH: OFF

## Touchscreen - CST816S

| Signal | ESP32-S3 GPIO |
|---|---:|
| SDA | 11 |
| SCL | 7 |
| RESET | 6 |

I2C address: 0x15
I2C clock: 100 kHz

## Restrictions

Do not modify GPIO assignments without hardware validation.