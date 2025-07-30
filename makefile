BINDIR:= bin
SRCDIR:= compiler
INCDIR:= $(SRCDIR)/includes
LIBNAME:= libjuve.a
LIBJUVE:= $(BINDIR)/$(LIBNAME)
SRC:= $(shell find ./$(SRCDIR) -type f -name '*.c')
CPPSRC:= $(wildcard juve/*.cpp)
CJUVE_SRC := $(wildcard juve/*.c)
CJUVE_OBJ := $(patsubst juve/%.c, $(BINDIR)/%.o, $(CJUVE_SRC))
CPPOBJ:= $(patsubst juve/%.cpp, $(BINDIR)/%.o, $(CPPSRC))
OBJ:= $(addprefix $(BINDIR)/, $(addsuffix .o, $(basename $(notdir $(SRC)))))
FLAGS:= -I$(INCDIR) -I. -I$(SRCDIR) -Wall -Wextra -pedantic -Wno-gnu-zero-variadic-macro-arguments -ggdb

CC := clang
CXX := clang++

TESTFILES := $(wildcard tests/*.kd)
TESTOUTPUTS := $(patsubst tests/%.kd, tests/out/%.test, $(TESTFILES))

TARGET:= $(BINDIR)/kudoc

all: $(TARGET)

$(TARGET): $(OBJ) $(LIBJUVE)
	$(CXX) -o $@ $^ $(FLAGS)

$(LIBJUVE): $(CPPOBJ) $(CJUVE_OBJ)
	ar -rcs $@ $^

$(BINDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c -o $@ $< $(FLAGS)

$(BINDIR)/%.o: $(SRCDIR)/*/%.c
	$(CC) -c -o $@ $< $(FLAGS)	

$(BINDIR)/%.o: juve/%.cpp
	$(CXX) -c -o $@ $< $(FLAGS)

$(BINDIR)/%.o: juve/%.c
	$(CC) -c -o $@ $< $(FLAGS)

clean:
	rm $(shell find -type f -name "*~") $(LIBJUVE) $(OBJ) $(CPPOBJ)

tests/out/%.test: tests/%.kd
	@mkdir -p tests/out
	$(TARGET) --test -i $< > $@ 2>&1
	@valgrind --leak-check=full --log-file=$@.valgrind $(TARGET) --test $< $@

test: $(TESTOUTPUTS)
	@echo "Tested: " $(TESTOUTPUTS)

.PHONY: clean test
