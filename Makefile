COMP = g++
CFLAGS = -std=c++2a -Wall -Wextra -pedantic -O2 -mmacosx-version-min=11 -stdlib=libc++
LFLAGS = -Iinclude/raylib/include -lraylib -Linclude/raylib \
-framework iokit -framework Cocoa -framework OpenGL -lm
SRC = main.cpp game.cpp textbasedgame.cpp \
textbox.cpp
PLATFORM = mac


all: clean build run

main: $(SRC)
	@$(COMP) $(CFLAGS) $^ -o build/$(PLATFORM)/$@ $(LFLAGS)
	@cp -rf ./assets build/$(PLATFORM)/

build: main
#	@printf "Compiling...\n"

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
	@rm -rf dist/mac/*
	@rm -rf dist/win/*