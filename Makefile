CC = gcc
CFLAGS = -g -I./src/linux -I./src/common
SRC = src/linux/utils.c src/linux/trash.c src/common/utils.c trashbhuwan.c
OBJ = $(SRC:.c=.o)
TARGET = trashbhuwan

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)
