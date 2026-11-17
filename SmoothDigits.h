#pragma once

#include <Arduino_GFX_Library.h>
#include <stdio.h>
#include <string.h>
#include "SmoothDigitsData.h"

namespace SmoothDigits {
// Optional canvas backing store; enables antialiasing over the current image.
inline uint16_t *&targetBuffer() {
  static uint16_t *buffer = nullptr;
  return buffer;
}
// Blend in RGB565 channels, retaining all 16 coverage levels at the edges.
inline uint16_t blend(uint16_t fg, uint16_t bg, int alpha) {
  int r = (((fg >> 11) & 31) * alpha + ((bg >> 11) & 31) * (15-alpha) + 7) / 15;
  int g = (((fg >> 5) & 63) * alpha + ((bg >> 5) & 63) * (15-alpha) + 7) / 15;
  int b = ((fg & 31) * alpha + (bg & 31) * (15-alpha) + 7) / 15;
  return (r << 11) | (g << 5) | b;
}

inline void coverage(Arduino_GFX &gfx, int x, int y, uint16_t color,
                     uint16_t fallback, int alpha) {
  if (x < 0 || x >= 240 || y < 0 || y >= 240 || alpha == 0) return;
  const uint16_t background = targetBuffer() ? targetBuffer()[y * 240 + x] : fallback;
  gfx.drawPixel(x, y, alpha == 15 ? color : blend(color, background, alpha));
}

// Blend over the active canvas, or a solid fallback. Never enlarge raster pixels.
inline void draw(Arduino_GFX &gfx, unsigned int value, int cx, int cy,
                 int maxWidth, int maxHeight, uint16_t color, uint16_t background,
                 bool rotate180 = false) {
  char text[12];
  snprintf(text, sizeof(text), "%u", value);
  const int count = strlen(text);
  const Face *face = nullptr;
  for (const Face &candidate : faces) {
    if (candidate.height <= maxHeight && count * (candidate.width + 2) - 2 <= maxWidth) {
      face = &candidate;
      break;
    }
  }
  if (!face) return; // Nonnegative ESP32 int values fit the 120/144-pixel PV areas.
  const int totalWidth = count * (face->width + 2) - 2;
  const int x = cx - totalWidth / 2;
  const int y = cy - face->height / 2;
  const int stride = (face->width * face->height + 1) / 2;
  for (int d = 0; d < count; ++d) {
    const uint8_t *mask = face->data + (text[d] - '0') * stride;
    for (int row = 0; row < face->height; ++row) {
      for (int col = 0; col < face->width; ++col) {
        int index = row * face->width + col;
        uint8_t packed = pgm_read_byte(mask + index / 2);
        int alpha = (index & 1) ? (packed & 15) : (packed >> 4);
        if (alpha) {
          const int dx = d * (face->width + 2) + col;
          coverage(gfx, x + (rotate180 ? totalWidth - 1 - dx : dx),
                   y + (rotate180 ? face->height - 1 - row : row), color, background, alpha);
        }
      }
    }
  }
}
} // namespace SmoothDigits
