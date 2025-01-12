#include "millis.h"
#include <SDL2/SDL.h>

u64 millis(void)
{
	return SDL_GetTicks64();
}
