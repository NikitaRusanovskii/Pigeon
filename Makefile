CC = gcc
CFLAGS = -Werror -Wextra -Wall -O2 -I./include/transport
LDFLAGS = -lcurl

TARGET = test_connector

SRCS = src/connector.c tests/test_connector.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	rm -f $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET)