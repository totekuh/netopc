CC=i686-w64-mingw32-gcc
CFLAGS=-Wall
SRC_DIR=src
DIST_DIR=dist

all: setup $(DIST_DIR)/netopc.exe

setup:
	mkdir -p $(DIST_DIR)

$(DIST_DIR)/netopc.exe: $(SRC_DIR)/main.o $(SRC_DIR)/exec.o
	$(CC) $(CFLAGS) -o $@ $^ -lws2_32

$(SRC_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(SRC_DIR)/exec.o: $(SRC_DIR)/exec.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(SRC_DIR)/*.o $(DIST_DIR)/netopc.exe
