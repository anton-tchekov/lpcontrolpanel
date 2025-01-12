#ifndef __GFX_H__
#define __GFX_H__

#include "types.h"

extern i32 _char_width, _char_height;

void gfx_main(const char *title);
void gfx_rect(i32 x, i32 y, i32 w, i32 h, u32 color);
void gfx_char(i32 x, i32 y, i32 c, u32 fg);
void gfx_string(i32 x, i32 y, u32 fg, const char *s);
void gfx_string_len(i32 x, i32 y, u32 fg, const char *s, i32 len);
void gfx_rect_border(i32 x, i32 y, i32 w, i32 h, i32 border, u32 color);
i32 gfx_string_width(const char *s);
i32 gfx_string_width_len(const char *s, i32 len);
void gfx_bg(void);
u32 gfx_color(i32 r, i32 g, i32 b);

#endif
