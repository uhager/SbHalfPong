## Makefile for SbHalfPong
## author: Ulrike Hager

SDL_INCLUDES = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf

CXX = g++
CXXFLAGS += -O2 -fpic -Wall -std=c++11 -I.
DEBUG_FLAGS = -g -DDEBUG 

OBJS = SbTexture.o SbTimer.o SbWindow.o SbObject.o SbMessage.o
PONGOBJS = $(OBJS) SbHalfPong.o
MAZEOBJS = $(OBJS) SbMaze.o

all: $(OBJS) pong maze
pong: $(PONGOBJS) SbHalfPong
maze: $(MAZEOBJS) SbMaze

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: all

.PHONY: clean

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SDL_INCLUDES) -o $@ -c $<


SbHalfPong: $(PONGOBJS) 
	$(CXX) $(CXXFLAGS) $(PONGOBJS) $(SDL_INCLUDES) $(SDL_LIBS) -o $@

SbMaze: $(MAZEOBJS) 
	$(CXX) $(CXXFLAGS) $(MAZEOBJS) $(SDL_INCLUDES) $(SDL_LIBS) -o $@

clean:
	rm -f *.o *.so $(OBJS) SbHalfPong SbMaze
