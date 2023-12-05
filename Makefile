CC = gcc
CFLAGS = -I./headers 
LDFLAGS = -lncurses

SRC_DIR = src
HDR_DIR = headers

SRCS = main.c $(SRC_DIR)/menu.c $(SRC_DIR)/renderGame.c $(SRC_DIR)/author.c $(SRC_DIR)/rollDice.c $(SRC_DIR)/vectorPawn.c $(SRC_DIR)/playerTurn.c $(SRC_DIR)/move.c $(SRC_DIR)/utils.c $(SRC_DIR)/bidirectionalList.c $(SRC_DIR)/menuUtils.c $(SRC_DIR)/renderGameUtils.c $(SRC_DIR)/fileActions.c
OBJS = $(SRCS:.c=.o)

TARGET = moj_program

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(TARGET) $(OBJS)