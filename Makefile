CC = gcc
TARGET = trashbhuwan
CFLAGS = -g -Wall

ifeq ($(OS),Windows_NT)
    PLATFORM = windows
    CFLAGS += -I./src/windows -I./src/common
    SRC = src/windows/utils.c src/windows/trash.c src/common/utils.c trashbhuwan.c
    LIBS += -lole32 -lshell32 -lshlwapi -luuid
else
    PLATFORM = linux
    CFLAGS += -I./src/linux -I./src/common
    SRC = src/linux/utils.c src/linux/trash.c src/common/utils.c trashbhuwan.c
endif

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

clean:
    ifeq ($(OS),Windows_NT)
	    del /Q $(subst /,\,$(OBJ)) $(subst /,\,$(TARGET))
    else
	    rm -rf $(OBJ) $(TARGET)
    endif
