#ifndef __LABEL_H__
#define __LABEL_H__

#include "types.h"

typedef enum
{
	ALIGN_LEFT,
	ALIGN_CENTER,
	ALIGN_RIGHT
} Align;

typedef struct
{
	i32 X, Y;
	Align Align;
	char *Text;
} Label;

void label_render(Label *label);

#endif
