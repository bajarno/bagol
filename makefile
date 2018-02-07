#!/bin/bash
OS = $(shell uname -s)

#OBJS specifies which files to compile as part of the project
OBJS = main.c

#CC specifies which compiler we're using
CC = gcc

#COMPILER_FLAGS specifies the additional compilation options we're using
# - suppresses all warnings
COMPILER_FLAGS = -g

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_ttf -lm

#INCLUDE_FLAGS specifies the directories of header files that should be included
ifeq ($(OS), Linux)
	INCLUDE_FLAGS = -I /usr/include/SDL2/
endif
ifeq ($(OS), Darwin) # Mac os
	INCLUDE_FLAGS = -I /Library/Frameworks/SDL2.framework/Headers/ -I /Library/Frameworks/SDL2_ttf.framework/Headers/
endif

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = bagol

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(INCLUDE_FLAGS) -o $(OBJ_NAME)