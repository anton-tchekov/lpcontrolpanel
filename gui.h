#ifndef __GUI_H__
#define __GUI_H__

#include "types.h"
#include "keys.h"

#define FLAG_SELECTED         0x100
#define FLAG_HOVER            0x200
#define FLAG_ACTIVE           0x400

typedef enum
{
	ELEMENT_TYPE_BUTTON,
	ELEMENT_TYPE_TEXTBOX,
	ELEMENT_TYPE_LABEL,
	ELEMENT_TYPE_LAMP,
	ELEMENT_TYPE_SLIDER
} ElementType;

void gui_element_add(ElementType type, void *elem);
void gui_render(void);
i32 gui_event_key(u32 key, u32 chr, u32 state);
void gui_mouseup(i32 x, i32 y);
i32 gui_mousedown(i32 x, i32 y);
void gui_mousemove(i32 x, i32 y);

#endif
