COMP = clang++
CFLAGS = -std=c++2a -Wall -Wextra -pedantic -O2
LFLAGS = -lm -Iinclude -lraylib -L/usr/local/include/raylib/raylib/ -framework iokit -framework Cocoa -framework OpenGL
SRC = main.cpp game.cpp
PLATFORM = mac


all: clean build run

main: $(SRC)
	@$(COMP) $(CFLAGS) $^ -o build/$(PLATFORM)/$@ $(LFLAGS)
	@cp -rf ./assets build/$(PLATFORM)/

build: main
	@echo "Compiling..."

dist: clean build
	@cp -rf ./assets dist/$(PLATFORM)/
	@cp build/$(PLATFORM)/main dist/$(PLATFORM)/textbasedgame
	@echo "Build completed for $(PLATFORM)"

run: main
	@./build/$(PLATFORM)/main

clean:
	@rm -f build/mac/main
	@rm -rf build/mac/*.dSYM
	@rm -f build/win/main
	@rm -rf build/win/*.dSYM
	@clear
	@rm -rf dist/mac/*
	@rm -rf dist/win/*