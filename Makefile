# Claude Quest - raylib / C
#
#   make            build ./claude_quest  (finds raylib via pkg-config or Homebrew)
#   make run        build and run
#   make test       build and run the test suite
#   make shots      render every screen to shots/ (for checking the layout)
#   make RAYLIB_DIR=/path/to/raylib     use a raylib source checkout (static lib)

CC      ?= cc
CFLAGS  ?= -O2 -std=gnu11 -Wall -Wextra -Wno-missing-field-initializers
TARGET  := claude_quest
ENGINE  := src/content.c src/context.c src/game.c
SRC     := src/main.c $(ENGINE)
UNAME_S := $(shell uname -s)

ifdef RAYLIB_DIR
  RL_CFLAGS := -I$(RAYLIB_DIR)/src
  RL_LIBS   := $(RAYLIB_DIR)/src/libraylib.a
else
  RL_CFLAGS := $(shell pkg-config --cflags raylib 2>/dev/null)
  RL_LIBS   := $(shell pkg-config --libs raylib 2>/dev/null)
  ifeq ($(strip $(RL_LIBS)),)
    ifeq ($(UNAME_S),Darwin)
      BREW      := $(shell brew --prefix 2>/dev/null)
      RL_CFLAGS := -I$(BREW)/include
      RL_LIBS   := -L$(BREW)/lib -lraylib
    else
      RL_LIBS   := -lraylib
    endif
  endif
endif

ifeq ($(UNAME_S),Darwin)
  SYS_LIBS := -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo \
              -framework CoreAudio -framework AudioToolbox
else
  SYS_LIBS := -lGL -lm -lpthread -ldl -lrt -lX11
endif

all: $(TARGET)

$(TARGET): $(SRC) src/*.h
	$(CC) $(CFLAGS) $(RL_CFLAGS) -o $@ $(SRC) $(RL_LIBS) $(SYS_LIBS) -lm

run: $(TARGET)
	./$(TARGET)

test: tests/test_main.c $(ENGINE) src/*.h
	$(CC) $(CFLAGS) -o test_quest tests/test_main.c $(ENGINE) -lm
	./test_quest

shots: $(TARGET)
	mkdir -p shots
	./$(TARGET) --shots "$(CURDIR)/shots"

clean:
	rm -f $(TARGET) test_quest
	rm -rf shots

.PHONY: all run test shots clean
