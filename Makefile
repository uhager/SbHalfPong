## Makefile for SbHalfPong
## author: Ulrike Hager

SDL_INCLUDES = $(shell sdl2-config --cflags)
SDL_LIBS = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf

CXX = g++
CXXFLAGS += -O2 -Wall -std=c++11 
DEBUG_FLAGS = -g -DDEBUG 

OBJS = SbHalfPong


all: $(OBJS)

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: all

.PHONY: clean

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(SDL_INCLUDES) -o $@ -c $<


SbHalfPong: SbHalfPong.cpp
	$(CXX) $(CXXFLAGS) $< $(SDL_INCLUDES) $(SDL_LIBS) -o $@

clean:
	rm -f *.o *.so $(OBJS)
