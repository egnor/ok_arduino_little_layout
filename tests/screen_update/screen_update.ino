#include <ok_little_layout.h>
#include <ok_logging.h>
#include <U8g2lib.h>
#include <Wire.h>

using DisplayDriver = U8G2_SSD1306_128X64_NONAME_F_HW_I2C;
extern char const* const ok_logging_config = "*=DETAIL";

void setup() {
  Serial.begin(115200);
  Serial.println("starting setup...");
  Wire.begin();

  DisplayDriver display(U8G2_R0);
  display.begin();
  display.clearDisplay();
  display.setPowerSave(0);

  auto* const layout = new_ok_little_layout(display.getU8g2());
  layout->line_printf(0, "\f9Line 0");
  layout->line_printf(1, "\f9Line 1");
  layout->line_printf(2, "\f9Line 2");
  layout->line_printf(3, "\f9Line 3");
  layout->line_printf(4, "\f9Line 4");
  Serial.println("LAYOUT 1 READY");

  layout->line_printf(3, "\f13\bUpdated line 3");
  Serial.println("LAYOUT 2 READY");

  layout->line_printf(1, "\f13\bUpdated line 1");
  Serial.println("LAYOUT 3 READY");
  delete layout;
}

void loop() {
  Serial.println("looping...");
  delay(500);
}
