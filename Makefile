# Compilation
CFLAGS = -std=gnu11 -Wall -pedantic -O3 -Isrc
CC = clang
LDFLAGS = 
LIBS = 

# Debug 
CFLAGS += -g 
CFLAGS += -fsanitize=address -fno-omit-frame-pointer
LDFLAGS += -fsanitize=address

# Libs
# Voir SDL2 CM2 

# Fichiers
TARGET = main
TEST_TARGET = test
 
MAIN_SRC = src/main.c 
MAIN_OBJ = $(MAIN_SRC:.c=.o) 

ALL_SRCS = $(wildcard src/*/*.c) # /!\ ne prend pas les sous-dossiers de src
TEST_SRCS = $(filter %_test.c, $(ALL_SRCS))
COMMON_SRCS = $(filter-out %_test.c, $(ALL_SRCS))

COMMON_OBJS = $(COMMON_SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_BINS = $(TEST_SRCS:.c=)

# Par défaut
all: $(TARGET) $(TEST_TARGET)

$(TARGET): src/main.o $(COMMON_OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

$(TEST_BINS): %: %.o $(COMMON_OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

$(TEST_TARGET): $(TEST_BINS)
	@for test in $(TEST_BINS); do\
		echo "Test en cours : $$(basename $$test)"; \
		./$$test; \
	done

%.o : %.c  #On ne s'embête pas avec les dépendances en .h qui pose peu problèmes
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(COMMON_OBJS) $(TARGET) $(TEST_OBJS) $(TEST_BINS) $(MAIN_OBJ) $(TEST_TARGET)
