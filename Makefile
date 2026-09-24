CC = cc
CFLAGS = -Wall -Wextra -Werror -std=c11

TARGET = celestial
SRC = src/main.c src/server.c src/event_loop.c src/http.c src/connection.c src/http_parser.c src/request_handler.c src/event_handler.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: clean