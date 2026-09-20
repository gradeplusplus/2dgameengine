#######################################
#declare Makefile variable
#######################################
CC = g++
STD = -std=c++17
COMPILER_FLAGS = -Wall -Wfatal-errors
INCLUDE_PATH = -I./libs -I./libs/sol/include -I./libs/imgui
SRC_FILES = ./src/*.cpp \
						./src/Game/*.cpp \
						./src/Logger/*.cpp \
						./src/ECS/*.cpp \
						./src/AssetStore/*.cpp \
						./libs/imgui/imgui.cpp \
						./libs/imgui/imgui_draw.cpp \
						./libs/imgui/imgui_widgets.cpp \
						./libs/imgui/imgui_sdl.cpp
LINKER_FLAGS = -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.3 -lSDL2 -llua5.3
OBJ_NAME = gameengine

build:
	$(CC) $(COMPILER_FLAGS) $(STD) $(SRC_FILES) $(INCLUDE_PATH) $(LINKER_FLAGS) -o $(OBJ_NAME)
run:
	./$(OBJ_NAME)
clean:
	rm $(OBJ_NAME)
