#include "gfx.h"
#include "colors.h"
#include "theme.h"
#include "keys.h"
#include "layout.h"
#include "gui.h"
#include "util.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#define GFX_INITIAL_WIDTH     480
#define GFX_INITIAL_HEIGHT    320

static i32 _quit;
static char *_font_name = "terminus.ttf";
static i32 _font_size = 20;
i32 _char_width, _char_height;
static i32 _gfx_width, _gfx_height;
static SDL_Texture *_font;
static SDL_Window *_window;
static SDL_Renderer *_renderer;
static i32 _bgw, _bgh, _bgy;
static i32 down, valid_drag;
static double ox0 = 0.0, oy0 = 0.0, zoom = 1.0, mx, my;
static double prev_x, prev_y;
static double omx, omy;

#define NUM_CHARS 128

static i32 font_load(char *font, i32 size)
{
	SDL_Surface *chars[NUM_CHARS];
	TTF_Font *ttf = TTF_OpenFont(font, size);
	if(!ttf)
	{
		return 1;
	}

	SDL_Color color = { 255, 255, 255, 0 };
	char letter[2];
	letter[1] = '\0';

	memset(chars, 0, sizeof(chars));

	for(i32 i = 33; i <= 126; ++i)
	{
		letter[0] = i;
		chars[i] = TTF_RenderText_Blended(ttf, letter, color);
	}

	TTF_CloseFont(ttf);

	i32 max_h = 0, max_w = 0;
	for(i32 i = 0; i < NUM_CHARS; ++i)
	{
		SDL_Surface *s = chars[i];
		if(!s) { continue; }
		if(s->w > max_w)
		{
			max_w = s->w;
		}

		if(s->h > max_h)
		{
			max_h = s->h;
		}
	}

	_char_width = max_w;
	_char_height = max_h;

	SDL_Surface *img = IMG_Load("images/lp.png");
	if(!img)
	{
		fprintf(stderr, "Could not load image: %s\n", IMG_GetError());
		return 1;
	}

	SDL_Surface *surface = SDL_CreateRGBSurface(0,
		i32_max(_char_width * 16, img->w), _char_height * 8 + img->h,
		32, 0xff, 0xff00, 0xff0000, 0xff000000);

	_bgy = _char_height * 8;
	_bgw = img->w;
	_bgh = img->h;
	SDL_Rect src2 = { 0, 0, img->w, img->h };
	SDL_Rect dst2 = { 0, _bgy, img->w, img->h };
	SDL_BlitSurface(img, &src2, surface, &dst2);
	SDL_FreeSurface(img);

	for(i32 i = 0; i < NUM_CHARS; ++i)
	{
		SDL_Surface *s = chars[i];
		if(!s) { continue; }
		SDL_Rect src = { 0, 0, s->w, s->h };
		i32 x = (i & 0x0F) * _char_width;
		i32 y = (i >> 4) * _char_height;
		SDL_Rect dst = { x, y, s->w, s->h };
		SDL_BlitSurface(s, &src, surface, &dst);
		SDL_FreeSurface(s);
	}

	SDL_Rect rect =
	{
		0,
		0,
		_char_width,
		_char_height
	};

	SDL_FillRect(surface, &rect, COLOR_WHITE);

	_font = SDL_CreateTextureFromSurface(_renderer, surface);
	_font_size = size;
	SDL_FreeSurface(surface);
	return 0;
}

static void resize(i32 w, i32 h)
{
	_gfx_width = w;
	_gfx_height = h;
}

static void gfx_init(const char *title)
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Error initializing SDL; SDL_Init: %s\n", SDL_GetError());
		exit(1);
	}

	if(!(_window = SDL_CreateWindow(title,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		GFX_INITIAL_WIDTH, GFX_INITIAL_HEIGHT,
		SDL_WINDOW_RESIZABLE)))
	{
		printf("Error creating SDL_Window: %s\n", SDL_GetError());
		SDL_Quit();
		exit(1);
	}

	if(!(_renderer = SDL_CreateRenderer
		(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)))
	{
		printf("Error creating SDL_Renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(_window);
		SDL_Quit();
		exit(1);
	}

	if(TTF_Init())
	{
		printf("Loading initializing TTF: %s\n", TTF_GetError());
		SDL_DestroyRenderer(_renderer);
		SDL_DestroyWindow(_window);
		SDL_Quit();
		exit(1);
	}

	if(font_load(_font_name, _font_size))
	{
		printf("Loading font failed: %s\n", SDL_GetError());
		TTF_Quit();
		SDL_DestroyRenderer(_renderer);
		SDL_DestroyWindow(_window);
		SDL_Quit();
		exit(1);
	}

	SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
	resize(GFX_INITIAL_WIDTH, GFX_INITIAL_HEIGHT);
}

static void gfx_destroy(void)
{
	SDL_DestroyTexture(_font);
	TTF_Quit();
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

i32 color_r(u32 color)
{
	return (color >> 16) & 0xFF;
}

i32 color_g(u32 color)
{
	return (color >> 8) & 0xFF;
}

i32 color_b(u32 color)
{
	return color & 0xFF;
}

static void setcolor(u32 color)
{
	SDL_SetTextureColorMod(_font,
		color_r(color), color_g(color), color_b(color));
}

u32 gfx_color(i32 r, i32 g, i32 b)
{
	return (r << 16) | (g << 8) | b;
}

void scr2obj(double *ox, double *oy, double sx, double sy)
{
	*ox = (sx - ox0) / zoom;
	*oy = (sy - oy0) / zoom;
}

void obj2scr(double *sx, double *sy, double ox, double oy)
{
	*sx = ox0 + (ox * zoom);
	*sy = oy0 + (oy * zoom);
}

static void render_copy(SDL_Rect *src, SDL_Rect *dst)
{
	SDL_Rect ndst;
	double tx = dst->x;
	double ty = dst->y;
	obj2scr(&tx, &ty, tx, ty);
	ndst.x = tx;
	ndst.y = ty;

	double bx = dst->x + dst->w;
	double by = dst->y + dst->h;
	obj2scr(&bx, &by, bx, by);
	ndst.w = bx - ndst.x;
	ndst.h = by - ndst.y;

	SDL_RenderCopy(_renderer, _font, src, &ndst);
}

static void zoom_change(void)
{
	// TODO?
}

static void handle_scroll(i32 y)
{
	if(y < 0)
	{
		double mx0, my0;
		scr2obj(&mx0, &my0, mx, my);
		zoom /= 1.25;
		obj2scr(&mx0, &my0, mx0, my0);
		ox0 += mx - mx0;
		oy0 += my - my0;
	}
	else if(y > 0)
	{
		double mx0, my0;
		scr2obj(&mx0, &my0, mx, my);
		zoom *= 1.25;
		obj2scr(&mx0, &my0, mx0, my0);
		ox0 += mx - mx0;
		oy0 += my - my0;
	}

	zoom_change();
}

void gfx_bg(void)
{
	u32 fg = THEME_LIGHT;
	SDL_Rect src = { 0, _bgy, _bgw, _bgh };
	SDL_Rect dst = { 0, 0, _bgw, _bgh };

	SDL_SetTextureColorMod(_font, color_r(fg), color_g(fg), color_b(fg));
	render_copy(&src, &dst);
}

static uint32_t _convert_key(int32_t scancode, int32_t mod)
{
	uint32_t key = scancode;

	if(mod & (KMOD_LCTRL | KMOD_RCTRL))
	{
		key |= MOD_CTRL;
	}

	if(mod & KMOD_LALT)
	{
		key |= MOD_ALT;
	}

	if(mod & KMOD_RALT)
	{
		key |= MOD_ALT_GR;
	}

	if(mod & (KMOD_LGUI | KMOD_RGUI))
	{
		key |= MOD_OS;
	}

	if(mod & (KMOD_LSHIFT | KMOD_RSHIFT))
	{
		key |= MOD_SHIFT;
	}

	return key;
}

uint8_t key_to_codepoint(uint16_t k)
{
	uint8_t nomods = k & 0xFF;

	if(nomods == KEY_TAB)                             { return '\t'; }
	else if(nomods == KEY_BACKSPACE)                  { return '\b'; }
	else if(nomods == KEY_RETURN)                     { return '\n'; }
	else if(nomods == KEY_SPACE)                      { return ' '; }
	else if(k == (KEY_COMMA | MOD_SHIFT))             { return ';'; }
	else if(k == (KEY_COMMA))                         { return ','; }
	else if(k == (KEY_PERIOD | MOD_SHIFT))            { return ':'; }
	else if(k == (KEY_PERIOD))                        { return '.'; }
	else if(k == (KEY_SLASH | MOD_SHIFT))             { return '_'; }
	else if(k == (KEY_SLASH))                         { return '-'; }
	else if(k == (KEY_BACKSLASH | MOD_SHIFT))         { return '\''; }
	else if(k == (KEY_BACKSLASH))                     { return '#'; }
	else if(k == (KEY_R_BRACKET | MOD_SHIFT))         { return '*'; }
	else if(k == (KEY_R_BRACKET | MOD_ALT_GR))        { return '~'; }
	else if(k == (KEY_R_BRACKET))                     { return '+'; }
	else if(k == (KEY_NON_US_BACKSLASH | MOD_SHIFT))  { return '>'; }
	else if(k == (KEY_NON_US_BACKSLASH | MOD_ALT_GR)) { return '|'; }
	else if(k == KEY_NON_US_BACKSLASH)                { return '<'; }
	else if(k == (KEY_MINUS | MOD_SHIFT))             { return '?'; }
	else if(k == (KEY_MINUS | MOD_ALT_GR))            { return '\\'; }
	else if(k == (KEY_EQUALS | MOD_SHIFT))            { return '`'; }
	else if(k == KEY_GRAVE)                           { return '^'; }
	else if(nomods >= KEY_A && nomods <= KEY_Z)
	{
		uint8_t c = nomods - KEY_A + 'a';

		if(c == 'z') { c = 'y'; }
		else if(c == 'y') { c = 'z'; }

		if(k & MOD_ALT_GR)
		{
			if(c == 'q') { return '@'; }
		}

		if(k & MOD_SHIFT)
		{
			c = toupper(c);
		}

		return c;
	}
	else if(nomods >= KEY_1 && nomods <= KEY_0)
	{
		static const uint8_t numbers[] =
			{ '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };

		static const uint8_t numbers_shift[] =
			{ '!', '\"', 0, '$', '%', '&', '/', '(', ')', '=' };

		static const uint8_t numbers_altgr[] =
			{ 0, 0, 0, 0, 0, 0, '{', '[', ']', '}' };

		uint8_t idx = nomods - KEY_1;

		if(k & MOD_SHIFT)
		{
			return *(numbers_shift + idx);
		}
		else if(k & MOD_ALT_GR)
		{
			return *(numbers_altgr + idx);
		}
		else
		{
			return *(numbers + idx);
		}
	}

	return 0;
}

void gfx_main(const char *title)
{
	gfx_init(title);
	layout_init();
	while(!_quit)
	{
		SDL_Event e;
		while(SDL_PollEvent(&e))
		{
			switch(e.type)
			{
			case SDL_WINDOWEVENT:
				if(e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					resize(e.window.data1, e.window.data2);
				}
				break;

			case SDL_KEYDOWN:
				if(e.key.keysym.sym == SDLK_ESCAPE)
				{
					_quit = 1;
					break;
				}

				u32 key = _convert_key(e.key.keysym.scancode, e.key.keysym.mod);
				if(gui_event_key(key, key_to_codepoint(key),
					e.key.repeat ? KEYSTATE_REPEAT : KEYSTATE_PRESSED))
				{
					break;
				}

				switch(e.key.keysym.sym)
				{
				case SDLK_r:
					zoom = 1;
					ox0 = 0;
					oy0 = 0;
					zoom_change();
					break;
				
				case SDLK_w:
					oy0 += zoom * 100;
					break;
				
				case SDLK_a:
					ox0 += zoom * 100;
					break;
				
				case SDLK_s:
					oy0 -= zoom * 100;
					break;
				
				case SDLK_d:
					ox0 -= zoom * 100;
					break;
				}
				break;

			case SDL_MOUSEMOTION:
				mx = e.motion.x;
				my = e.motion.y;

				scr2obj(&omx, &omy, mx, my);
				gui_mousemove(omx, omy);

				if(down && valid_drag)
				{
					ox0 += (mx - prev_x);
					oy0 += (my - prev_y);
				}

				prev_x = mx;
				prev_y = my;
				break;

			case SDL_MOUSEBUTTONDOWN:
				valid_drag = gui_mousedown(omx, omy);
				if(e.button.button != SDL_BUTTON_LEFT) { break; }
				down = 1;
				break;
			
			case SDL_MOUSEBUTTONUP:
				gui_mouseup(omx, omy);
				if(e.button.button != SDL_BUTTON_LEFT) { break; }
				down = 0;
				break;

			case SDL_MOUSEWHEEL:
				handle_scroll(e.wheel.y);
				break;

			case SDL_QUIT:
				_quit = 1;
				break;
			}
		}

		SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
		SDL_RenderClear(_renderer);

		layout_render();

		SDL_RenderPresent(_renderer);
	}

	gfx_destroy();
}

void gfx_rect(i32 x, i32 y, i32 w, i32 h, u32 color)
{
	SDL_Rect blank = { 0, 0, _char_width, _char_height };
	SDL_Rect blank_dst = { x, y, w, h };
	setcolor(color);
	render_copy(&blank, &blank_dst);
}

void gfx_rect_border(i32 x, i32 y, i32 w, i32 h, i32 border, u32 color)
{
	gfx_rect(x, y, w, border, color);
	gfx_rect(x, y + h - border, w, border, color);

	gfx_rect(x, y + border, border, h - 2 * border, color);
	gfx_rect(x + w - border, y + border, border, h - 2 * border, color);
}

i32 gfx_string_width(const char *s)
{
	return strlen(s) * _char_width;
}

i32 gfx_string_width_len(const char *s, i32 len)
{
	return len * _char_width;
	(void)s;
}

void gfx_char(i32 x, i32 y, i32 c, u32 fg)
{
	if(c == ' ') { return; }
	SDL_Rect src = { (c & 0x0F) * _char_width, (c >> 4) * _char_height, _char_width, _char_height };
	SDL_Rect dst = { x, y, _char_width, _char_height };
	SDL_SetTextureColorMod(_font, color_r(fg), color_g(fg), color_b(fg));
	render_copy(&src, &dst);
}

void gfx_string(i32 x, i32 y, u32 fg, const char *s)
{
	for(i32 c; (c = *s); ++s)
	{
		gfx_char(x, y, c, fg);
		x += _char_width;
	}
}

void gfx_string_len(i32 x, i32 y, u32 fg, const char *s, i32 len)
{
	for(i32 c, i = 0; i < len && (c = *s); ++s, ++i)
	{
		gfx_char(x, y, c, fg);
		x += _char_width;
	}
}
