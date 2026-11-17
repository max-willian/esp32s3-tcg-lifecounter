#pragma once

#include "SmoothDigits.h"
#include "SmoothIconsData.h"

namespace SmoothIcons {
inline void mask(Arduino_GFX &gfx, const uint8_t *data, int size, int x, int y,
                 uint16_t foreground, uint16_t background) {
  for (int row = 0; row < size; ++row) {
    for (int col = 0; col < size; ++col) {
      int index = row * size + col;
      uint8_t packed = pgm_read_byte(data + index / 2);
      int alpha = (index & 1) ? packed & 15 : packed >> 4;
      if (alpha) SmoothDigits::coverage(gfx, x + col, y + row, foreground, background, alpha);
    }
  }
}

inline void button(Arduino_GFX &gfx, const uint8_t *icon, int cx, int cy,
                   uint16_t color, uint16_t surface, uint16_t background) {
  mask(gfx, plate, 36, cx - 18, cy - 18, surface, background);
  mask(gfx, icon, 24, cx - 12, cy - 12, color, surface);
}
} // namespace SmoothIcons
