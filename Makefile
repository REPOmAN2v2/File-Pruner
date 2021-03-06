ODIR=./obj
BDIR=./bin
SRC=./src

CC = gcc
CFLAGS = -Wall -static -Wextra -pthread -std=c11

ifeq ($(DEBUG), 1)
CFLAGS += -g -pg
else
CFLAGS += -O3 -march=native
endif

ifdef COMSPEC
CFLAGS += -D__USE_MINGW_ANSI_STDIO=1
endif

_HEADERS = directory.h file.h thpool.h gc.h
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

pruner: ${OBJECTS}
	${CC} $^ $(SRC)/main.c $(CFLAGS) -o $(BDIR)/$@

$(ODIR)/%.o: $(SRC)/%.c $(SRC)/%.h build
	${CC} $< -c $(CFLAGS) -o $@

.PHONY: default clean check dist distcheck install rebuild uninstall