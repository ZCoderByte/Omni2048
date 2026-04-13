DBGDIR = debug
RELDIR = release
TESTDIR = tests
TESTS = $(TESTDIR)/test_move_tiles_right $(TESTDIR)/test_combine_tiles $(TESTDIR)/test_move_tile $(TESTDIR)/test_move_tiles_up $(TESTDIR)/test_move_tiles_left
EXE = 2048

IO_BACKEND = curses

ifeq ($(IO_BACKEND), curses)
	LIBS = -lncurses -ltinfo
	MACROS = -DIO_CURSES
	IO_SRCS = io_curses.h
endif

ifeq ($(IO_BACKEND), stdio)
	MACROS = -DIO_STDIO
	IO_SRCS = io_stdio.h
endif

SRCS = main.c 2048.h $(IO_SRCS)

CFLAGS = -Wall -Wextra $(MACROS)
DBGCFLAGS = $(CFLAGS) -g -O0 -DDEBUG
RELCFLAGS = $(CFLAGS) -Os -DNDEBUG -s

all: release

.PHONY: tests

$(TESTS): $(TESTDIR)/%: $(TESTDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

tests: $(TESTS) $(SRCS)
	for t in $(TESTS); do ./$$t || exit 1; done

debug: $(DBGDIR)/$(EXE)
release: $(RELDIR)/$(EXE)

$(DBGDIR)/$(EXE): $(SRCS)
	mkdir -p $(DBGDIR)
	$(CC) $(DBGCFLAGS) -o $@ main.c $(LIBS)

$(RELDIR)/$(EXE): $(SRCS)
	mkdir -p $(RELDIR)
	$(CC) $(RELCFLAGS) -o $@ main.c $(LIBS)

clean:
	rm -rf $(DBGDIR) $(RELDIR) $(TESTS)
