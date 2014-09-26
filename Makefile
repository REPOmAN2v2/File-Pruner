ODIR=./obj
BDIR=./bin
SRC=./src

CC = gcc
CFLAGS = -Wall -Wextra -pthread -std=c11

ifeq ($(DEBUG), 1)
CFLAGS += -g -pg
else
CFLAGS += -O3 -march=native
endif

_HEADERS = directory.h file.h
_OBJECTS := $(_HEADERS:.h=.o)
OBJECTS = $(patsubst %,$(ODIR)/%,$(_OBJECTS))
HEADERS = $(patsubst %,$(SRC)/%,$(_HEADERS))

all: pruner

build:
	@test -d $(ODIR) || mkdir $(ODIR)
	@test -d $(BDIR) || mkdir $(BDIR)

clean:
	rm -rf ./obj/
	rm -rf ./bin/
	rm -f ./*~
	rm -f ./*.swp

rebuild: clean default

pruner: ${OBJECTS} $(SRC)/include.h
	${CC} $^ $(SRC)/main.c $(CFLAGS) -o $(BDIR)/$@

$(ODIR)/%.o: $(SRC)/%.c $(SRC)/%.h $(SRC)/include.h build
	${CC} $< -c $(CFLAGS) -o $@

.PHONY: default clean check dist distcheck install rebuild uninstall