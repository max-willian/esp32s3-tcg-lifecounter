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

## Menu Button (BOOT)

- GPIO0, active LOW, INPUT_PULLUP.
- The existing BOOT button is read as an additional menu input during normal
  operation; no display/touch GPIO assignments are changed.
- Identification is based on the matching SpotPear reference board configuration:
  https://raw.githubusercontent.com/78/xiaozhi-esp32/main/main/boards/spotpear/sp-esp32-s3-1.28-box/config.h
- The manufacturer identifies the second button as BOOT:
  https://spotpear.com/index.php/wiki/ESP32-S3-N16R8-AI-DeepSeek-XiaoZhi-XiaGe-Qwen-DouBao-1.28-inch-Round-LCD-BOX-TouchScreen.html
- Confirm on the physical BOX-THIN revision. This has not been physically tested.
- GPIO0 is a boot strap: holding BOOT while powering on/resetting can enter the
  ROM download mode. Use a short press after the counter has started.

## Restrictions

Do not modify GPIO assignments without hardware validation.
