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
SRCS = $(wildcard src/*.c) # /!\ ne prend pas les sous-dossiers de src
OBJS = $(SRCS:.c=.o)

# Par défaut
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

%.o : %.c  #On ne s'embête pas avec les dépendances en .h qui pose peu problèmes
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
