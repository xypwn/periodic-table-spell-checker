CC=cc
SRC=main.c
EXE=main

all: $(EXE)

$(EXE): $(SRC)
	$(CC) $(SRC) -o $(EXE)

.PHONY: clean

clean:
	rm -f $(EXE)
