# OK Little Layout for Arduino

<p>
<img width="410" alt="Microcontroller connected to display showing a tabular layout" src="https://github.com/user-attachments/assets/159f64de-037c-4e3f-bee7-94674864c930" />
</p>

The Little Layout library for Arduino lays out lines of text for small monochrome displays using [the u8g2 display driver library](https://github.com/olikraus/u8g2/wiki), with incremental update. Simple markup controls font size, boldness, inverse video, and column layout.

You should first consider these more established options:
- [LVGL](https://lvgl.io/) - full-color, high-end interactive GUI for fast 32-bit microcontrollers
- [GUIslice](https://github.com/ImpulseAdventure/GUIslice) - like LVGL, not as pretty but runs on lower spec hardware
- [UiUiUi](https://github.com/dirkhillbrecht/UiUiUi) - similar to Little Layout, also based on u8g2, more widget-box oriented and less text oriented; dormant?
- also: [µGUI](https://github.com/achimdoebler/UGUI), [micro-gui](https://github.com/ryankurte/micro-gui), [micrOS](https://github.com/GeoSn0w/micrOS), [RTT-GUI](https://github.com/onelife/RTT-GUI), [Tgui](https://github.com/NordicAlliance/arduino-tgui), [MicroGUI-Embedded](https://github.com/microgui/MicroGUI-Embedded?tab=readme-ov-file)

Little Layout is intended to be very simple and work well on very small displays to support the "Everything Has A Little Status Display" philosophy, not to be a full interactive user interface (but you could build a menu system using it). It uses [the "Everyday Pixel" fonts](https://github.com/egnor/everyday_pixel_fonts/) by default, but can be configured with any u8g2-compatible fonts.

## Usage

First, initialize a U8g2 driver for your hardware; [see the U8g2 setup documentation](https://github.com/olikraus/u8g2/wiki/u8g2setupcpp).

Then, create an `OkLittleLayout` object, passing your U8g2 driver:
```
OkLittleLayout* my_little_layout = new_ok_little_layout(my_u8g2.getU8g2());
```

Finally write text (printf-style) to numbered display lines of your choice:
```
my_little_layout->line_printf(0, "Hello World! The time is: %.3f", millis() * 1e-3f);
```

Little Layout updates the display so it always shows all text lines in order. Line positioning depends on font height (see below). Enough lines are available to fill the screen even at the smallest font size; writing past that point has no effect.

## Markup

## Examples

These use a [128x64 SSD1306 display](https://www.adafruit.com/product/326). _[Try this code on wokwi!](https://wokwi.com/projects/446633047263763457)_

<table><tr><td>

  ```
  // "Hello World" for OK Little Layout

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

</td>
<td>
  <img width="565" lt="Screen with layout demonstration" src="https://github.com/user-attachments/assets/e575ede4-64f3-4289-b426-1738af726687" />
  <p align="center">(nice)</p>
</td></tr>
<tr></tr>
<tr><td>

  ```
  // Font sizes (only showing the layout->line_printf calls)
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

  ```
  // Text styles
  layout->line_printf(0, "\f9\bBold\b text");
  layout->line_printf(1, "\f9\vInverse Video\v text");
  ```

</td><td>
  <img width="226" alt="Screen with bold and inverse text" src="https://github.com/user-attachments/assets/f0f096bc-8173-46a9-8e5a-32545916bfb9" />
</td></tr>
<tr></tr>
<tr><td>

  ```
  // Column layout with tabs
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

  ```
  // Demo from the header
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
