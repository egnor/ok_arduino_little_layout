# OK Little Layout for Arduino

<p>
<img width="410" alt="Microcontroller connected to display showing a tabular layout" src="https://github.com/user-attachments/assets/159f64de-037c-4e3f-bee7-94674864c930" />
</p>


This Arduino library lays out lines of text for small displays using [the u8g2 display driver library](https://github.com/olikraus/u8g2/wiki), with incremental update. Simple markup controls font size, boldness, inverse video, and column layout.

This library uses the ["Everyday Pixel" fonts](https://github.com/egnor/everyday_pixel_fonts/) by default, but can be configured with any u8g2-compatible fonts.

## Examples

This is for a [128x64 SSD1306 display](https://www.adafruit.com/product/326) on default I2C pins. _[Watch this code run on wokwi!](https://wokwi.com/projects/446633047263763457)_

```
#include <ok_little_layout.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0);
OkLittleLayout* layout;
int count = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  display.begin();
  display.clearDisplay();
  display.setPowerSave(0);

  layout = new_ok_little_layout(display.getU8g2());
  layout->line_printf(0, "\f13\bBig Bold Text!");
  layout->line_printf(1, "\f7Small text / \bin bold\b / \f5tiny text");
  layout->line_printf(2, "\f4");
  layout->line_printf(3, "\bCol 1\tCol 2\tCol 3");
  layout->line_printf(4, "\f11ABC\t\f9%.2f\t\f7good job", 1.23);
  layout->line_printf(5, "\f11XYZ\t\f9%.2f\t\f7no comment", 2.34);
  layout->line_printf(6, "\v\2INVERSE VIDEO FOR YOUR PLEASURE");
}

void loop() {
  delay(500);
  layout->line_printf(0, "\f13\bBig Bold Text!\b %d", ++count);
}
```
