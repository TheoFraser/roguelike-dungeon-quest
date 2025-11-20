CC = gcc
CFLAGS = -Wall -Wextra -g -Wno-format-truncation
LDFLAGS = -lncurses -lm

SOURCES = main.c game.c player.c enemy.c item.c dungeon.c ability.c inventory.c render.c fov.c save.c utils.c status.c shop.c dungeon_features.c random_events.c ranged.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = roguelike

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run