// make sure the library compiles with these warnings on
#pragma GCC diagnostic error "-Wformat=2"

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
  layout->line_printf(0, "\f5Hello, \bWorld 5!");
  layout->line_printf(1, "\f7Hello, \bWorld 7!");
  layout->line_printf(2, "\f9Hello, \bWorld 9!");
  layout->line_printf(3, "\f11Hello, \bWorld 11!");
  layout->line_printf(4, "\f13Hello, \bWorld 13!");
  layout->line_printf(5, "Tab 1\tTab 2\tTab 3");
  layout->line_printf(6, "\vReverse Video");
  Serial.println("LAYOUT READY");
  delete layout;
}

void loop() {
  Serial.println("looping...");
  delay(500);
}
