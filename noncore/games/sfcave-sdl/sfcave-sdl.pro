TEMPLATE	= app
CONFIG		+= qt warn_on release
DESTDIR		= $(OPIEDIR)/bin 
TARGET		= sfcave-sdl

DEFINES = _REENTRANT main=SDL_main

INCLUDEPATH += $(OPIEDIR)/include
INCLUDEPATH += $(OPIEDIR)/include/SDL
DEPENDPATH  += $(OPIEDIR)/include

LIBS += -lqpe -L${SDLDIR}/lib -lSDL -lSDLmain -lSDL_gfx -lSDL_image -lSDL_mixer -lstdc++ 

SOURCES = 	animatedimage.cpp \
		bfont.cpp \
		font.cpp \
		game.cpp \
		menu.cpp \
		help.cpp \
		player.cpp \
		random.cpp \
		sfcave.cpp \
		sfcave_game.cpp \
		gates_game.cpp \
		fly_game.cpp \
		flyterrain.cpp \
		sound.cpp \
		terrain.cpp \
		settings.cpp \
		starfield.cpp \
		util.cpp

HEADERS = 	animatedimage.h \
		bfont.h \
		constants.h \
		font.h \
		game.h \
		menu.h \
		player.h \
		random.h \
		rect.h \
		sfcave.h \
		help.h \
		sfcave_game.h \
		gates_game.h \
		fly_game.h \
		flyterrain.h \
		sound.h \
		terrain.h \
		stringtokenizer.h \
		settings.h \
		starfield.h \
		util.h

include ( ../../../include.pro ) 