#include "gui.h"
#include "button.h"
#include "textbox.h"
#include "label.h"
#include "slider.h"
#include "lamp.h"

typedef struct
{
	ElementType Type;
	u32 Flags;
	union
	{
		Label *L;
		Button *B;
		Textbox *T;
		Slider *S;
		Lamp *A;
	};
} Element;

#define MAX_ELEMENTS 128

static Element Elements[MAX_ELEMENTS];
static i32 Count = 0, Hover = -1, Selected = -1;

void gui_element_add(ElementType type, void *elem)
{
	Element *e = Elements + Count;
	e->Type = type;
	switch(type)
	{
	case ELEMENT_TYPE_BUTTON:
		e->B = elem;
		break;

	case ELEMENT_TYPE_TEXTBOX:
		e->T = elem;
		break;

	case ELEMENT_TYPE_LABEL:
		e->L = elem;
		break;
	}

	++Count;
}

static void element_render(Element *e, u32 flags)
{
	switch(e->Type)
	{
	case ELEMENT_TYPE_LABEL:
		label_render(e->L);
		break;

	case ELEMENT_TYPE_BUTTON:
		button_render(e->B, flags);
		break;

	case ELEMENT_TYPE_TEXTBOX:
		textbox_render(e->T, flags);
		break;
	}
}

void gui_render(void)
{
	for(i32 i = 0; i < Count; ++i)
	{
		u32 flags = 0;
		if(Selected == i) { flags |= FLAG_SELECTED; }
		if(Hover == i) { flags |= FLAG_HOVER; }
		element_render(Elements + i, flags);
	}
}

static i32 element_hover(Element *e, i32 x, i32 y)
{
	switch(e->Type)
	{
	case ELEMENT_TYPE_BUTTON:
		return button_bounds(e->B, x, y);

	case ELEMENT_TYPE_TEXTBOX:
		return textbox_bounds(e->T, x, y);

	default:
		break;
	}

	return 0;
}

void gui_mousemove(i32 x, i32 y)
{
	for(i32 i = 0; i < Count; ++i)
	{
		if(element_hover(Elements + i, x, y))
		{
			Hover = i;
			return;
		}
	}

	Hover = -1;
}

void gui_mouseup(i32 x, i32 y)
{
	gui_mousemove(x, y);
}

i32 gui_mousedown(i32 x, i32 y)
{
	gui_mousemove(x, y);
	Selected = Hover;
	if(Selected < 0)
	{
		return 1;
	}

	Element *ce = Elements + Selected;
	if(ce->Type == ELEMENT_TYPE_BUTTON)
	{
		Button *b = ce->B;
		if(b->Click)
		{
			b->Click();
		}
	}
	else if(ce->Type == ELEMENT_TYPE_TEXTBOX)
	{
		textbox_click(ce->T, x);
	}

	return 0;
}

i32 gui_event_key(u32 key, u32 chr, u32 state)
{
	if(Selected < 0)
	{
		return 0;
	}

	if(state == KEYSTATE_RELEASED)
	{
		return 0;
	}

	Element *ce = Elements + Selected;
	if(ce->Type == ELEMENT_TYPE_TEXTBOX)
	{
		textbox_event_key(ce->T, key, chr);
		return 1;
	}

	return 0;
}
