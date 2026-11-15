# Display

## Controller

GC9A01A

## Resolution

240 x 240 pixels.

## Library

Arduino_GFX_Library

## Working Initialization

```cpp
Arduino_DataBus *bus = new Arduino_ESP32SPI(
    47, 5, 4, 2, GFX_NOT_DEFINED
);

Arduino_GFX *gfx = new Arduino_GC9A01(
    bus, 38, 0, true
);
```

## RGB565 Colors

| Name | Value |
|---|---|
| Background | 0x4208 |
| White | 0xFFFF |
| Green | 0x2645 |
| Red | 0xF800 |

## Notes

The display is round. UI elements must respect its circular
visible area and avoid placing important content near corners.