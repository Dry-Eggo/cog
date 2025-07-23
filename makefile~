BINDIR:= out
SRCDIR:= source
INCDIR:= includes
SRC:= $(wildcard $(SRCDIR)/*.c)
CPPSRC:= $(wildcard $(SRCDIR)/khm/*.cpp)
CPPOBJ:= $(patsubst $(SRCDIR)/khm/%.cpp, $(BINDIR)/%.o, $(CPPSRC))
OBJ:= $(patsubst $(SRCDIR)/%.c, $(BINDIR)/%.o, $(SRC))
FLAGS:= -I$(INCDIR) -I./source/khm

TARGET:= $(BINDIR)/kudoc

all: $(TARGET)

$(TARGET): $(OBJ) $(CPPOBJ)
	clang++ -o $@ $^ $(FLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.c
	clang -c -o $@ $< $(FLAGS)

$(BINDIR)/%.o: $(SRCDIR)/khm/%.cpp
	clang++ -c -o $@ $< $(FLAGS)
