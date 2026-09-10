CC = cc
CFLAGS = -Wall -Wextra -Werror -std=c11

TARGET = celestial
SRC = src/main.c src/server.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean