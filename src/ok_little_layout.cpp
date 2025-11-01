#include "ok_little_layout.h"

#include <algorithm>
#include <cstdarg>
#include <cstring>

#include <ok_logging.h>
#include <U8g2lib.h>

static const OkLoggingContext OK_CONTEXT("ok_little_layout");

namespace {

class OkLittleLayoutDef : public OkLittleLayout {
 public:
  OkLittleLayoutDef(
    u8g2_t* u8g2,
    uint8_t const* (*choose_font)(int, bool)
  ) : u8g2(u8g2), choose_font(choose_font) {
    nonblank_y = u8g2->height;  // Erase any existing content
    rows_size = u8g2->height / 6 + 1;
    rows = new Row[rows_size];
    for (int i = 0; i < rows_size; ++i) rows[i].top_y = -1;
  }

  virtual ~OkLittleLayoutDef() {
    for (int i = 0; i < rows_size; ++i) {
      delete[] rows[i].text;
    }
    delete[] rows;
    delete[] temp;
  }

  virtual void line_printf(int line, char const* format, ...) override {
    if (line < 0 || line >= rows_size) return;
    auto& row = rows[line];

    while (true) {
      va_list args;
      va_start(args, format);
      auto const len = vsnprintf(temp, temp_size, format, args);
      va_end(args);

      if (len >= temp_size) {
        delete[] temp;
        temp_size = len + 1;
        temp = new char[temp_size];
      } else if (len < 0) {
        OK_ERROR("Bad LittleLayout format (line %d): %s", line, format);
        return;
      } else {
        break;
      }
    }

    if (row.text == nullptr || strcmp(temp, row.text) != 0) {
      int const old_height = row.height;
      row.height = 0;
      row.baseline = 0;

      delete[] row.text;
      row.text = temp;
      temp = nullptr;
      temp_size = 0;

      TextChunk chunk = {row.text, row.text};
      while (next_chunk(&chunk)) {
        row.height = std::max(row.height, chunk.height);
        uint8_t const* font = choose_font(chunk.height, chunk.bold);
        if (font != nullptr && chunk.end > chunk.begin) {
          u8g2_SetFont(u8g2, font);
          int const ascent = u8g2->font_ref_ascent;
          row.baseline = std::max(row.baseline, ascent + 1);
        }
      }
      row.tabs = chunk.tabs;

      int bot_y = draw_one_line(rows[line]);
      if (row.height != old_height) {
        for (int i = line + 1; i < rows_size; ++i) {
          rows[i].top_y = bot_y;
          bot_y = draw_one_line(rows[i]);
        }
        if (bot_y < nonblank_y) {
          int const blank_h = nonblank_y - bot_y;
          u8g2_SetDrawColor(u8g2, 0);
          u8g2_DrawBox(u8g2, 0, bot_y, u8g2->width, blank_h);
          nonblank_y = bot_y;
          bot_y += blank_h;
        }
      }

      push_rect(0, row.top_y, u8g2->width, bot_y - row.top_y);
    }
  }

  virtual u8g2_t* get_u8g2() const override { return this->u8g2; }

 private:
  static constexpr int MAX_HEIGHT = 15;

  struct Row {
    int top_y = 0;
    int height = 0;
    int baseline = 0;
    int tabs = 0;
    char* text = nullptr;
  };

  struct TextChunk {
    char const* begin = nullptr;
    char const* end = nullptr;
    int tabs = 0;
    int height = 8;
    bool bold = false;
    bool inverse = false;
  };

  u8g2_t* const u8g2;
  uint8_t const* (*choose_font)(int, bool);

  Row* rows = nullptr;
  int rows_size = 0;
  char* temp = nullptr;
  int temp_size = 0;
  int nonblank_y = 0;

  int draw_one_line(Row const& row) {
    if (row.height == 0) return row.top_y;

    u8g2_SetFontMode(u8g2, 1);  // For _tr fonts
    u8g2_SetFontPosBaseline(u8g2);

    int text_x = -10;
    int const text_y = row.top_y + row.baseline;
    int last_tabs = -1;
    TextChunk chunk = {row.text, row.text};
    while (next_chunk(&chunk)) {
      if (chunk.end <= chunk.begin) continue;
      uint8_t const* font = choose_font(chunk.height, chunk.bold);
      if (font == nullptr) continue;

      u8g2_SetFont(u8g2, font);
      int const first_w = u8g2_GetGlyphWidth(u8g2, *chunk.begin);
      int const first_offset = u8g2->glyph_x_offset;

      int const prev_x = text_x;
      if (chunk.tabs > last_tabs) {
        int const tab_x = (chunk.tabs * u8g2->width) / (row.tabs + 1);
        text_x = std::max(text_x, tab_x - first_offset);
        last_tabs = chunk.tabs;
      }

      int end_x = text_x + first_w;
      for (char const* c = chunk.begin + 1; c < chunk.end; ++c) {
        end_x += (*c <= 5) ? *c : u8g2_GetGlyphWidth(u8g2, *c);
      }

      u8g2_SetDrawColor(u8g2, chunk.inverse ? 1 : 0);
      u8g2_DrawBox(u8g2, prev_x, row.top_y, end_x - prev_x, row.height);

      u8g2_SetDrawColor(u8g2, chunk.inverse ? 0 : 1);
      for (char const* c = chunk.begin; c < chunk.end; ++c) {
        text_x += (*c <= 5) ? *c : u8g2_DrawGlyph(u8g2, text_x, text_y, *c);
      }
    }

    if (text_x < u8g2->width) {
      u8g2_SetDrawColor(u8g2, chunk.inverse ? 1 : 0);
      u8g2_DrawBox(u8g2, text_x, row.top_y, u8g2->width - text_x, row.height);
    }

    int const bot_y = std::min<int>(row.top_y + row.height, u8g2->height);
    if (bot_y > nonblank_y) nonblank_y = bot_y;
    return bot_y;
  }

  static bool next_chunk(TextChunk* ch) {
    ch->begin = ch->end;
    for (;;) {
      for (ch->end = ch->begin; *ch->end != '\0'; ++ch->end) {
        if (*ch->end < 0x20 && *ch->end > 5) break;
      }
      if (ch->end > ch->begin) return true;
      if (*ch->end == '\0') return false;

      switch (*ch->begin++) {
        case '\b':
          ch->bold = !ch->bold;
          break;
        case '\f':
          if (*ch->begin >= '0' && *ch->begin <= '9') {
            ch->height = *ch->begin++ - '0';
            if (ch->height <= 2 && *ch->begin >= '0' && *ch->begin <= '9') {
              ch->height = ch->height * 10 + (*ch->begin++ - '0');
            }
          }
          break;
        case '\t':
          ++ch->tabs;
          break;
        case '\v':
          ch->inverse = !ch->inverse;
          break;
        default:
          ch->end = ch->begin + 1;
          return true;
      }
    }
  }

  void push_rect(int x, int y, int w, int h) {
    // TODO: account for rotation if any
    int const tx = std::min<int>(x, u8g2->width) / 8;
    int const ty = std::min<int>(y, u8g2->height) / 8;
    int const tw = std::min<int>(x + w + 7, u8g2->width) / 8 - tx;
    int const th = std::min<int>(y + h + 7, u8g2->height) / 8 - ty;
    u8g2_UpdateDisplayArea(u8g2, tx, ty, tw, th);
  }
};

}  // namespace

OkLittleLayout* new_ok_little_layout(
  u8g2_t* u8g2,
  uint8_t const* (*choose_font)(int, bool)
) {
  OK_FATAL_IF(u8g2 == nullptr);
  OK_FATAL_IF(choose_font == nullptr);
  return new OkLittleLayoutDef(u8g2, choose_font);
}
