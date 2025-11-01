#include "ok_little_layout.h"

#include <everyday_pixel_fonts.h>
#include <U8g2lib.h>

uint8_t const* default_ok_little_layout_font(int height, bool bd) {
  if (height < 5) return nullptr;

  switch (height) {
    case 5:
    case 6: return bd ? everyday_tiny_black_u8g2 : everyday_tiny_u8g2;
    case 7:
    case 8: return bd ? everyday_slight_black_u8g2 : everyday_slight_u8g2;
    case 9: return bd ? everyday_standard_black_u8g2 : everyday_standard_u8g2;
    case 10:
    case 11: return bd ? everyday_typical_black_u8g2 : everyday_typical_u8g2;
    case 12:
    case 13: return bd ? everyday_ample_black_u8g2 : everyday_ample_u8g2;
    default: return bd ? everyday_vast_black_u8g2 : everyday_vast_u8g2;
  }

  /*
  switch (height) {
    case 5: return u8g2_font_3x3basic_tr;
    case 6: return u8g2_font_u8glib_4_tr;
    case 7: return bd ? u8g2_font_wedge_tr : u8g2_font_tiny5_tr;
    case 8: return bd ? u8g2_font_squeezed_b6_tr
                        : u8g2_font_squeezed_r6_tr;
    case 9:
    case 10: return bd ? u8g2_font_NokiaSmallBold_tr
                         : u8g2_font_NokiaSmallPlain_tr;
    case 11: return bd ? u8g2_font_helvB08_tr : u8g2_font_helvR08_tr;
    case 12: return bd ? u8g2_font_crox1hb_tr : u8g2_font_crox1h_tr;
    case 13:
    case 14: return bd ? u8g2_font_crox2hb_tr : u8g2_font_crox2h_tr;
    default: return bd ? u8g2_font_helvB10_tr : u8g2_font_helvR10_tr;
  }
  */
}
