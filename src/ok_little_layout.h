// Simple text layout for small mono screens via U8g2

#pragma once

#include <cstdint>

extern "C" struct u8g2_struct;

class OkLittleLayout {
 public:
  virtual ~OkLittleLayout() = default;
  virtual void line_printf(int line, char const* format, ...) = 0;
  virtual u8g2_struct* get_u8g2() const = 0;
};

uint8_t const* default_ok_little_layout_font(int size, bool bold);

OkLittleLayout* new_ok_little_layout(
  u8g2_struct* u8g2,
  uint8_t const* (*choose_font)(int, bool) = default_ok_little_layout_font
);
