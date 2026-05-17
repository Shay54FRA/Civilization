# Compilation
CFLAGS = -std=gnu11 -Wall -pedantic -O3 -Isrc
CC = clang
LDFLAGS = 
LIBS = 

# Recuperation automatique des flags SDL2
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LIBS := $(shell sdl2-config --libs)

# Debug 
CFLAGS += -g 
CFLAGS += -fsanitize=address -fno-omit-frame-pointer
LDFLAGS += -fsanitize=address

# On ajoute la librairie SDL2 + SDL2_gfx
CFLAGS += $(SDL_CFLAGS)
LIBS = $(SDL_LIBS) -lSDL2_gfx -lm -lncurses


# Fichiers
TARGET = civ
TEST_TARGET = test
 
MAIN_SRC = src/civ.c 
MAIN_OBJ = $(MAIN_SRC:.c=.o) 

ALL_SRCS = $(wildcard src/*/*.c) # /!\ ne prend pas les sous-dossiers de src
TEST_SRCS = $(filter %_test.c, $(ALL_SRCS))
COMMON_SRCS = $(filter-out %_test.c, $(ALL_SRCS))

COMMON_OBJS = $(COMMON_SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_BINS = $(TEST_SRCS:.c=)

# Par défaut
all: $(TARGET) $(TEST_TARGET)

$(TARGET): src/civ.o $(COMMON_OBJS)
	$(CC) $^ $(LDFLAGS) $(LIBS) -o $@

$(TEST_BINS): %: %.o $(COMMON_OBJS)
	$(CC) $^ $(LDFLAGS) $(LIBS) -o $@

$(TEST_TARGET): $(TEST_BINS)
	@for test in $(TEST_BINS); do\
		echo "Test en cours : $$(basename $$test)"; \
		./$$test; \
	done

%.o : %.c  #On ne s'embête pas avec les dépendances en .h qui pose peu problèmes
	$(CC) $(CFLAGS) -c $< -o $@ 

req:
	sudo apt update
	sudo apt install libsdl2-dev
	sudo apt install libsdl2-gfx-dev
	sudo apt-get install libncurses5-dev libncursesw5-dev

clean:
	rm -f $(COMMON_OBJS) $(TARGET) $(TEST_OBJS) $(TEST_BINS) $(MAIN_OBJ) $(TEST_TARGET)
