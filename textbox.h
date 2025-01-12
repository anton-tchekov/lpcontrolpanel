#ifndef __TEXTBOX_H__
#define __TEXTBOX_H__

#include "types.h"

typedef struct TEXTBOX
{
	i32 X, Y, W;
	i32 Selection, Position, Length;
	char *Text;
	void (*Enter)(void);
} Textbox;

void textbox_render(Textbox *i, u32 flags);
i32 textbox_bounds(Textbox *e, i32 x, i32 y);
void textbox_click(Textbox *i, i32 x);
void textbox_event_key(Textbox *i, u32 key, u32 chr);

#endif
