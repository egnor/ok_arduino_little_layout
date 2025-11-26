#include <ok_little_layout.h>
#include <ok_logging.h>
#include <U8g2lib.h>
#include <Wire.h>

using DisplayDriver = U8G2_SSD1306_128X64_NONAME_F_HW_I2C;
extern char const* const ok_logging_config = "*=DETAIL";

static void draw_rotated(u8g2_cb_t const* rot) {
  DisplayDriver display(rot);
  display.begin();
  display.clearDisplay();
  display.setPowerSave(0);

  auto* const layout = new_ok_little_layout(display.getU8g2());
  layout->line_printf(0, "\f9Line 0");
  layout->line_printf(1, "\f9Line 1");
  layout->line_printf(2, "\f9Line 2");
  layout->line_printf(3, "\f9Line 3");
  layout->line_printf(4, "\f9Line 4");
  layout->line_printf(3, "\f13\bUpdated line 3");
  delete layout;
}

void setup() {
  Serial.begin(115200);
  Serial.println("starting setup...");
  Wire.begin();

  draw_rotated(U8G2_R0); Serial.println("R0 LAYOUT READY");
  draw_rotated(U8G2_R1); Serial.println("R1 LAYOUT READY");
  draw_rotated(U8G2_R2); Serial.println("R2 LAYOUT READY");
  draw_rotated(U8G2_R3); Serial.println("R3 LAYOUT READY");

  draw_rotated(U8G2_MIRROR); Serial.println("MIRROR LAYOUT READY");
  draw_rotated(U8G2_MIRROR_VERTICAL);
  Serial.println("MIRROR VERTICAL LAYOUT READY");
}

void loop() {
  Serial.println("looping...");
  delay(500);
}
