FILES = \
	alloc.c \
	button.c \
	gfx.c \
	gui.c \
	label.c \
	layout.c \
	log.c \
	main.c \
	millis.c \
	mqtt.c \
	lamp.c \
	slider.c \
	textbox.c \
	util.c

all:
	gcc \
		$(FILES) \
		-Wall -Wextra -Wshadow -std=c11 -pedantic \
		-o controlpanel \
		-g \
		-lSDL2 -lSDL2_ttf -lSDL2_image -lpthread -lm -lpaho-mqtt3c

win:
	x86_64-w64-mingw32-gcc \
		$(FILES) \
		-Wall -Wextra -Wshadow -std=c11 -pedantic \
		-o controlpanel.exe \
		-g \
		-lSDL2 -lSDL2_ttf -lSDL2_image -lpthread -lm -lpaho-mqtt3c

run:
	./controlpanel

clean:
	rm -rf controlpanel
