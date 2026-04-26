# Compilation
CFLAGS = -std=gnu11 -Wall -pedantic -O3
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
TEST = main_test               
SRCS = $(wildcard src/*.c) # /!\ ne prend pas les sous-dossiers de src
OBJS = $(filter-out src/main.o src/main_test.o, $(SRCS:.c=.o)) #On ne comprends pas les 2 exec pour éviter la répétition de main

# Par défaut
all: $(TARGET) $(TEST)

$(TARGET): src/main.o $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

$(TEST): src/main_test.o $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

%.o : %.c  #On ne s'embête pas avec les dépendances en .h qui pose peu problèmes
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(TEST) src/main.o src/main_test.o
