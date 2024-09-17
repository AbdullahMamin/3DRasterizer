CC := gcc
ERRFLAGS := -Wall -Wextra -Wpedantic -Werror
LIBS := -lSDL2 -lSDL2_image -lSDL2_mixer -llua5.4 -lm
OPT := -O3
CCFLAGS := -std=c11

OUTPUT := bin
SOURCE_DIR := ./src

SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))
DEPENDS := $(patsubst %.c, %.d, $(SOURCES))

.PHONY: all clean package install

all: $(OUTPUT)

install: all
	sudo mv $(OUTPUT) /usr/bin/makina2d

clean:
	rm -rf $(OUTPUT) $(OBJECTS) $(DEPENDS)

package: clean
	rm project.7z
	7z a -t7z -m0=lzma -mx=9 -mfb=64 -md=32m -ms=on project.7z src res Makefile
	
$(OUTPUT): $(OBJECTS)
	$(CC) $(CCFLAGS) $(ERRFLAGS) $(OPT) $^ -o $@ $(LIBS)

-include $(DEPENDS)

%.o: %.c Makefile
	$(CC) $(CCFLAGS) $(ERRFLAGS) $(OPT) -MMD -MP -c $< -o $@ $(LIBS)
