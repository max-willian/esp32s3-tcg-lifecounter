#pragma once

#include <Arduino_GFX_Library.h>
#include <math.h>

// Fixed 24x24 canvas. Two-pixel strokes with rounded endpoints.
namespace UIIcons {
enum Icon { Settings, Refresh, Plus, Minus, Check, Close, Back };

inline void stroke(Arduino_GFX &g, int x1, int y1, int x2, int y2,
                   uint16_t color) {
  int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
  int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
  int error = dx + dy;
  for (;;) {
    g.fillCircle(x1, y1, 1, color);
    if (x1 == x2 && y1 == y2) break;
    int twice = 2 * error;
    if (twice >= dy) { error += dy; x1 += sx; }
    if (twice <= dx) { error += dx; y1 += sy; }
  }
}

inline void draw(Arduino_GFX &g, Icon icon, int x, int y, uint16_t color) {
  const int cx = x + 12, cy = y + 12;
  if (icon == Plus || icon == Minus) {
    stroke(g, x + 6, cy, x + 18, cy, color);
    if (icon == Plus) stroke(g, cx, y + 6, cx, y + 18, color);
  } else if (icon == Check) {
    stroke(g, x + 5, y + 12, x + 10, y + 17, color);
    stroke(g, x + 10, y + 17, x + 19, y + 7, color);
  } else if (icon == Close) {
    stroke(g, x + 6, y + 6, x + 18, y + 18, color);
    stroke(g, x + 18, y + 6, x + 6, y + 18, color);
  } else if (icon == Back) {
    stroke(g, x + 15, y + 6, x + 9, cy, color);
    stroke(g, x + 9, cy, x + 15, y + 18, color);
  } else if (icon == Refresh) {
    for (int angle = -45; angle < 225; angle += 10) {
      float a = angle * PI / 180.0f;
      float b = (angle + 10) * PI / 180.0f;
      stroke(g, cx + lroundf(8 * cosf(a)), cy + lroundf(8 * sinf(a)),
             cx + lroundf(8 * cosf(b)), cy + lroundf(8 * sinf(b)), color);
    }
    stroke(g, x + 6, y + 6, x + 5, y + 12, color);
    stroke(g, x + 6, y + 6, x + 12, y + 6, color);
  } else {
    // Closed toothed outline with a hollow center.
    for (int i = 0; i < 32; ++i) {
      float a = i * PI / 16.0f, b = (i + 1) * PI / 16.0f;
      int r1 = (i % 4 == 1 || i % 4 == 2) ? 10 : 8;
      int j = (i + 1) % 32;
      int r2 = (j % 4 == 1 || j % 4 == 2) ? 10 : 8;
      stroke(g, cx + lroundf(r1 * cosf(a)), cy + lroundf(r1 * sinf(a)),
             cx + lroundf(r2 * cosf(b)), cy + lroundf(r2 * sinf(b)), color);
    }
    g.drawCircle(cx, cy, 3, color);
    g.drawCircle(cx, cy, 4, color);
  }
}
} // namespace UIIcons
