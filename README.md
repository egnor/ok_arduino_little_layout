# OK Little Layout for Arduino

<p>
<img width="410" alt="Microcontroller connected to display showing a tabular layout" src="https://github.com/user-attachments/assets/159f64de-037c-4e3f-bee7-94674864c930" />
</p>

OK Little Layout is an Arduino library to format text on small monochrome displays using [the u8g2 display driver library](https://github.com/olikraus/u8g2/wiki). Simple markup controls font size, boldness, inverse video, and column layout; changes render incrementally.

Before using this library, first consider more established options:
- [LVGL](https://lvgl.io/) - full-color, high-end interactive GUI for 32-bit microcontrollers with framebuffers
- [GUIslice](https://github.com/ImpulseAdventure/GUIslice) - like LVGL, not as pretty but runs on lower spec hardware
- [UiUiUi](https://github.com/dirkhillbrecht/UiUiUi) - similar to this library, also u8g2-based, more widget-boxy and less text-printy; dormant?
- also: [µGUI](https://github.com/achimdoebler/UGUI), [micro-gui](https://github.com/ryankurte/micro-gui), [micrOS](https://github.com/GeoSn0w/micrOS), [RTT-GUI](https://github.com/onelife/RTT-GUI), [Tgui](https://github.com/NordicAlliance/arduino-tgui), [MicroGUI-Embedded](https://github.com/microgui/MicroGUI-Embedded?tab=readme-ov-file)

OK Little Layout is designed for pervasive small status displays, not to be a full interactive user interface (but you could build a menu system using it). It uses [the "Everyday Pixel" fonts](https://github.com/egnor/everyday_pixel_fonts/) by default, but can use any u8g2-compatible fonts.

## Usage

First [install U8g2](https://github.com/olikraus/u8g2/wiki/u8g2install) and [initialize a U8g2 driver for your hardware](https://github.com/olikraus/u8g2/wiki/u8g2setupcpp#introduction).

Then, install this library ("OK Little Layout" in the library manager), and include its header:
```
#include <ok_little_layout.h>
```

Then, create an `OkLittleLayout` object, passing your U8g2 driver:
```
OkLittleLayout* my_little_layout = new_ok_little_layout(my_display.getU8g2());
```

Finally, write text (printf-style) to numbered display lines of your choice:
```
my_little_layout->line_printf(0, "Hello World! The time is: %.3f", millis() * 1e-3f);
```

OK Little Layout updates the display so it always shows all text lines in order, to the limits of the screen. Line positioning depends on each line's maximum font height (see below). Each line will run off the right edge without wrapping, and line numbers run off the bottom without scrolling.

### Markup

Font size, text style, and spacing are set by control characters in the `->line_printf(...)` text:
- `\f<size>` - set font size so lines take `<size>` pixels, from `\f5` ~ `\f15` (`\f1` ~ `\f20` for blank lines)
- `\1` ~ `\6` - add precise horizontal spacing in pixels
- `\t` - advance one tabstop; each line is evenly divided by tabs in that line (see examples below)
- `\b` - toggle boldface on and off
- `\v` - toggle inverse video on and off

These are C string escapes that generate control characters. The control characters are the markup.

### Configuring fonts

By default, OK Little Layout uses [the "Everyday Pixel" fonts](https://github.com/egnor/everyday_pixel_fonts/). If you want to use different fonts, or just avoid linking in sizes you don't use, write a font-choosing function:
```
uint8_t const* my_font_chooser(int size, bool bold) {
  if (size < 5) return nullptr;
  switch (size) {  // basic example...
    case 5: return u8g2_font_3x3basic_tr;
    case 6: return u8g2_font_u8glib_4_tr;
    case 7: return bd ? u8g2_font_wedge_tr : u8g2_font_tiny5_tr;
    case 8: return bd ? u8g2_font_squeezed_b6_tr : u8g2_font_squeezed_r6_tr;
    default: return bd ? u8g2_font_NokiaSmallBold_tr : u8g2_font_NokiaSmallPlain_tr;
  }
}
```

Then, pass your function as the optional second parameter to `new_ok_little_layout`:
```
my_little_layout = new_ok_little_layout(my_u8g2.getU8g2(), my_font_chooser);
```

See [`default_ok_little_layout_font.cpp`](src/default_ok_little_layout_font.cpp) for the default font selection.

## Examples

These use a [128x64 SSD1306 display](https://www.adafruit.com/product/326).

<table>
<tr><td>
  <p align="center">
    <b>Hello World</b><br/>
    <i><a href="https://wokwi.com/projects/446633047263763457">(try this example on wokwi)</a></i>
  </p>
  
  ```
  #include <ok_little_layout.h>
  #include <U8g2lib.h>
  #include <Wire.h>
  
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0);
  OkLittleLayout* layout;
  int count = 0;
  
  void setup() {
    Serial.begin(115200);
    Wire.begin();  // Assume default I2C pins
    display.begin();
    display.clearDisplay();
    display.setPowerSave(0);
    layout = new_ok_little_layout(display.getU8g2());
    layout->line_printf(1, "Pleased to meet you.");
  }
  
  void loop() {
    layout->line_printf(0, "Hello, World! %d", ++count);
    delay(500);
  }
  ```

</td><td>
  <img width="565" lt="Screen with layout demonstration" src="https://github.com/user-attachments/assets/e575ede4-64f3-4289-b426-1738af726687" />
  <p align="center">(nice)</p>
</td></tr>
<tr></tr>
<tr><td>
  <p align="center">
    <b>Font sizes</b><br/>
    <i>(program structure omitted for brevity...)</i>
  </p>

  ```
  layout->line_printf(0, "\f55px text");
  layout->line_printf(1, "\f77px text");
  layout->line_printf(2, "\f99px text");
  layout->line_printf(3, "\f1111px text");
  layout->line_printf(4, "\f1313px text");
  layout->line_printf(5, "\f1515px text");
  ```

</td><td>
  <img width="226" alt="Screen with font sizes from 5-15px" src="https://github.com/user-attachments/assets/ef714c67-f325-4ab6-977d-3d5efc903793" />
</td></tr>
<tr></tr>
<tr><td>
  <p align="center"><b>Font styles</b></p>

  ```
  layout->line_printf(0, "\f9\bBold\b text");
  layout->line_printf(1, "\f9\vInverse Video\v text");
  ```

</td><td>
  <img width="226" alt="Screen with bold and inverse text" src="https://github.com/user-attachments/assets/f0f096bc-8173-46a9-8e5a-32545916bfb9" />
</td></tr>
<tr></tr>
<tr><td>
  <p align="center"><b>Column layout using tabs</b></p>

  ```
  layout->line_printf(0, "First \tSecond");
  layout->line_printf(1, "Column \tColumn");
  layout->line_printf(2, "\f4");
  layout->line_printf(3, "First \tSecond \tThird");
  layout->line_printf(4, "Column \tColumn \tColumn");
  layout->line_printf(5, "\t<- blank ->\t");
  layout->line_printf(6, "\f4");
  layout->line_printf(7, "First\tSecond\tThird\tFourth");
  layout->line_printf(8, "1 \t2 Too Long \t3 \t4 ");
  ```

</td><td>
  <img width="226" alt="Screen with columns of text" src="https://github.com/user-attachments/assets/f272b07c-6a4b-4034-956a-53e22ee35eff" />
</td></tr>
<tr></tr>
<tr><td>
  <p align="center"><b>General formatting demo</b></p>

  ```
  layout->line_printf(0, "\f13\bBig Bold Text!");
  layout->line_printf(1, "\f7Small text / \bin bold\b / \f5tiny text");
  layout->line_printf(2, "\f4");
  layout->line_printf(3, "\bCol 1\tCol 2\tCol 3");
  layout->line_printf(4, "\f11ABC\t\f9%.2f\t\f7good job", 1.23);
  layout->line_printf(5, "\f11XYZ\t\f9%.2f\t\f7no comment", 2.34);
  layout->line_printf(6, "\v\2INVERSE VIDEO FOR YOUR PLEASURE");
  ```

</td><td>
  <img width="226" alt="Screen with layout demonstration" src="https://github.com/user-attachments/assets/2f96c6f1-4501-4143-a2f3-640e2fd4615f" />
</td></tr>
</table>
