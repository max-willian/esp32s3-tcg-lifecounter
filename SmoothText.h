#pragma once

#include "SmoothDigits.h"
#include "SmoothTextData.h"

namespace SmoothText {
// Only the application's fixed labels are stored.
// When adding a label, add it to GenerateText.ps1 and regenerate the assets.
inline const Label *find(const char *text, int height) {
  for (const Label &label : labels) {
    if (label.height == height && strcmp(label.text, text) == 0) return &label;
  }
  return nullptr;
}

inline int width(const char *text, int height) {
  const Label *label = find(text, height);
  return label ? label->width : 0;
}

inline void draw(Arduino_GFX &gfx, const char *text, int x, int y, int height,
                 uint16_t color, uint16_t background, bool rotate180 = false) {
  const Label *label = find(text, height);
  if (!label) return;
  for (int row = 0; row < label->height; ++row) {
    for (int col = 0; col < label->width; ++col) {
      int index = row * label->width + col;
      uint8_t packed = pgm_read_byte(label->data + index / 2);
      int alpha = (index & 1) ? (packed & 15) : (packed >> 4);
      if (alpha) SmoothDigits::coverage(gfx, x + (rotate180 ? label->width - 1 - col : col),
                                        y + (rotate180 ? label->height - 1 - row : row),
                                        color, background, alpha);
    }
  }
}
} // namespace SmoothText
