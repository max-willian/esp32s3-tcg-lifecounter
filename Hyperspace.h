#pragma once
#include "HyperspaceData.h"

namespace Hyperspace {
inline uint16_t duration(uint8_t frame) {
  return pgm_read_word(durations + frame);
}

inline void render(uint16_t *buffer, uint8_t frame) {
  uint16_t colors[16];
  for (int i = 0; i < 16; ++i) colors[i] = pgm_read_word(palette + i);
  const uint8_t *source = frames + uint32_t(frame) * FRAME_BYTES;
  for (uint32_t i = 0; i < FRAME_BYTES; ++i) {
    uint8_t packed = pgm_read_byte(source + i);
    buffer[i * 2] = colors[packed >> 4];
    buffer[i * 2 + 1] = colors[packed & 15];
  }
}
} // namespace Hyperspace
