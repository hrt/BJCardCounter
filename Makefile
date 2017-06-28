CC      = gcc
CFLAGS  = -std=c99 -Wall -Werror
SRC			= src/
OBJDIR	= obj
SRCDIR	= src
OUTDIR	= bin

OOBJ		= card_lib.o hand_lib.o

_DEPS		= card_lib.h hand_lib.h util.h tests.h
DEPS    = $(patsubst %,$(SRCDIR)/%,$(_DEPS))

_OBJ		= $(OOBJ) main.o
OBJ			= $(patsubst %,$(OBJDIR)/%,$(_OBJ))

_TOBJ		= $(OOBJ) tests.o hand_lib_tests.o card_lib_tests.o
TOBJ		=  $(patsubst %,$(OBJDIR)/%,$(_TOBJ))

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


$(OUTDIR)/Counter: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(OUTDIR)/Test:	$(TOBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean check all

clean:
	rm -f $(OBJDIR)/*.o $(OUTDIR)/Counter $(OUTDIR)/Test

check: $(OUTDIR)/Counter $(OUTDIR)/Test
	./$(OUTDIR)/Test

.PHONY: all clean
