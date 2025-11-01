#include "ok_little_layout.h"

#include <U8g2lib.h>

uint8_t const* default_ok_little_layout_font(int height, bool bold) {
  if (height < 5) return nullptr;
  switch (height) {
    case 5: return u8g2_font_3x3basic_tr;
    case 6: return u8g2_font_u8glib_4_tr;
    case 7: return bold ? u8g2_font_wedge_tr : u8g2_font_tiny5_tr;
    case 8: return bold ? u8g2_font_squeezed_b6_tr
                        : u8g2_font_squeezed_r6_tr;
    case 9:
    case 10: return bold ? u8g2_font_NokiaSmallBold_tr
                         : u8g2_font_NokiaSmallPlain_tr;
    case 11: return bold ? u8g2_font_helvB08_tr : u8g2_font_helvR08_tr;
    case 12: return bold ? u8g2_font_crox1hb_tr : u8g2_font_crox1h_tr;
    case 13:
    case 14: return bold ? u8g2_font_crox2hb_tr : u8g2_font_crox2h_tr;
    default: return bold ? u8g2_font_helvB10_tr : u8g2_font_helvR10_tr;
  }
}
