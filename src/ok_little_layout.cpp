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
    // for (int i = 0; i < rows_size; ++i) rows[i].top_y = -1;
  }

  virtual ~OkLittleLayoutDef() {
    for (int i = 0; i < rows_size; ++i) {
      delete[] rows[i].text;
    }
    delete[] rows;
    delete[] temp;
  }

  virtual void line_printf(int line, char const* format, ...) override {
    if (line < 0) {
      OK_ERROR("Bad line number L%d", line);
      return;
    }
    if (line >= rows_size) {
      OK_DETAIL("Offscreen L%d", line);
      return;
    }

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
        OK_ERROR("Bad L%d format: %s", line, format);
        return;
      } else {
        break;
      }
    }

    auto& row = rows[line];
    if (row.text != nullptr && strcmp(temp, row.text) == 0) {
      OK_DETAIL("Unchanged L%d: \"%s\"", line, temp);
      return;
    }

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
      if (chunk.end > chunk.begin) {
        if (font != nullptr) {
          u8g2_SetFont(u8g2, font);
          int const ascent = u8g2->font_ref_ascent;
          row.baseline = std::max(row.baseline, ascent + 1);
        } else {
          OK_ERROR(
            "No %dpx%s font, skipping L%d chunk: \"%.*s\"",
            chunk.height, chunk.bold ? " bold" : "", line,
            chunk.end - chunk.begin, chunk.begin
          )
        }
      }
    }
    row.tabs = chunk.tabs;
    OK_DETAIL(
      "L%d y=%d h=%d bl=%d ts=%d: \"%s\"",
      line, row.top_y, row.height, row.baseline, row.tabs, row.text
    );

    int next_y = draw_line(rows[line]);
    for (int i = line + 1; i < rows_size && next_y != rows[i].top_y; ++i) {
      if (rows[i].height > 0) {
        OK_DETAIL("Moving L%d y=%d -> %d", i, rows[i].top_y, next_y);
      }
      rows[i].top_y = next_y;
      next_y = draw_line(rows[i]);

      if (i == rows_size - 1 && next_y < nonblank_y) {
        int const blank_h = nonblank_y - next_y;
        u8g2_SetDrawColor(u8g2, 0);
        u8g2_DrawBox(u8g2, 0, next_y, u8g2->width, blank_h);
        nonblank_y = next_y;
        next_y += blank_h;
      }
    }

    push_rect(0, row.top_y, u8g2->width, next_y - row.top_y);
  }

  virtual u8g2_t* get_u8g2() const override { return this->u8g2; }

 private:
  static constexpr int MAX_HEIGHT = 15;

  struct Row {
    int top_y = -1;  // Compensate for inter-line spacing
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

  int draw_line(Row const& row) {
    if (row.height == 0) return row.top_y;

    u8g2_SetFontMode(u8g2, 1);  // For _tr fonts
    u8g2_SetFontPosBaseline(u8g2);

    int last_tabs = -1;  // Treat the left margin (tabs=0) as a tabstop
    int text_x = 0;
    int const text_y = row.top_y + row.baseline;
    TextChunk chunk = {row.text, row.text};
    while (next_chunk(&chunk)) {
      if (chunk.end <= chunk.begin) continue;
      uint8_t const* font = choose_font(chunk.height, chunk.bold);
      if (font == nullptr) continue;

      u8g2_SetFont(u8g2, font);

      int const prev_x = text_x;
      int end_x = text_x;
      for (char const* c = chunk.begin; c < chunk.end; ++c) {
        int const char_w = (*c <= 6) ? *c : u8g2_GetGlyphWidth(u8g2, *c);
        int const char_xo = (*c <= 6) ? 0 : u8g2->glyph_x_offset;
        if (chunk.tabs > last_tabs) {
          int const tab_x = (chunk.tabs * u8g2->width) / (row.tabs + 1);
          end_x = text_x = std::max(text_x, tab_x - char_xo);
          last_tabs = chunk.tabs;
        }
        end_x += char_w;
      }

      u8g2_SetDrawColor(u8g2, chunk.inverse ? 1 : 0);
      u8g2_DrawBox(u8g2, prev_x, row.top_y, end_x - prev_x, row.height);
      u8g2_SetDrawColor(u8g2, chunk.inverse ? 0 : 1);
      for (char const* c = chunk.begin; c < chunk.end; ++c) {
        text_x += (*c <= 6) ? *c : u8g2_DrawGlyph(u8g2, text_x, text_y, *c);
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
    auto const orig_begin = ch->begin = ch->end;
    for (;;) {
      for (ch->end = ch->begin; *ch->end != '\0'; ++ch->end) {
        if (*ch->end < 0x20 && *ch->end > 6) break;
      }
      if (ch->end > ch->begin) return true;
      if (*ch->begin == '\0') return (ch->begin > orig_begin);

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
          } else if (*ch->begin == 'b') {
            ++ch->begin;
            ch->bold = !ch->bold;
          } else if (*ch->begin == 'v') {
            ++ch->begin;
            ch->inverse = !ch->inverse;
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
    if (w <= 0 || h <= 0) return;
    int const tx_lim = u8g2_GetBufferTileWidth(u8g2) - 1;
    int const ty_lim = u8g2_GetBufferTileHeight(u8g2) - 1;
    int tx_0, tx_1, ty_0, ty_1;
    if (u8g2->cb == U8G2_R0) {
      tx_0 = x / 8; ty_0 = y / 8;
      tx_1 = (x + w - 1) / 8; ty_1 = (y + h - 1) / 8;
    } else if (u8g2->cb == U8G2_R1) {
      tx_0 = tx_lim - (y + h - 1) / 8; ty_0 = x / 8;
      tx_1 = tx_lim - y / 8; ty_1 = (x + w - 1) / 8;
    } else if (u8g2->cb == U8G2_R2) {
      tx_0 = tx_lim - (x + w - 1) / 8; ty_0 = ty_lim - (y + h - 1) / 8;
      tx_1 = tx_lim - x / 8; ty_1 = ty_lim - y / 8;
    } else if (u8g2->cb == U8G2_R3) {
      tx_0 = y / 8; ty_0 = ty_lim - (x + w - 1) / 8;
      tx_1 = (y + h - 1) / 8; ty_1 = ty_lim - x / 8;
    } else if (u8g2->cb == U8G2_MIRROR) {
      tx_0 = tx_lim - (x + w - 1) / 8; ty_0 = y / 8;
      tx_1 = tx_lim - x / 8; ty_1 = (y + h - 1) / 8;
    } else if (u8g2->cb == U8G2_MIRROR_VERTICAL) {
      tx_0 = x / 8; ty_0 = ty_lim - (y + h - 1) / 8;
      tx_1 = (x + w - 1) / 8; ty_1 = ty_lim - y / 8;
    } else {
      tx_0 = 0; ty_0 = 0; tx_1 = tx_lim; ty_1 = ty_lim;
    }

    if (tx_0 > tx_lim || ty_0 > ty_lim || tx_1 < 0 || ty_1 < 0) {
      OK_DETAIL(
        "No update: (%d,%d)+(%d,%d) => offscreen T(%d,%d)+(%d,%d)",
        x, y, w, h, tx_0, ty_0, tx_1 - tx_0 + 1, ty_1 - ty_0 + 1
      );
      return;
    }

    int const tx_clip = std::max(0, std::min(tx_lim, tx_0));
    int const ty_clip = std::max(0, std::min(ty_lim, ty_0));
    int const tw_clip = std::max(0, std::min(tx_lim, tx_1)) - tx_clip + 1;
    int const th_clip = std::max(0, std::min(ty_lim, ty_1)) - ty_clip + 1;
    OK_DETAIL(
      "Updating: (%d,%d)+(%d,%d) => clipped T(%d,%d)+(%d,%d)",
      x, y, w, h, tx_clip, ty_clip, tw_clip, th_clip
    );
    u8g2_UpdateDisplayArea(u8g2, tx_clip, ty_clip, tw_clip, th_clip);
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
