BINDIR:= bin
SRCDIR:= compiler
INCDIR:= includes
SRC:= $(wildcard $(SRCDIR)/*.c)
CPPSRC:= $(wildcard juve/*.cpp)
CPPOBJ:= $(patsubst juve/%.cpp, $(BINDIR)/%.o, $(CPPSRC))
OBJ:= $(patsubst $(SRCDIR)/%.c, $(BINDIR)/%.o, $(SRC))
FLAGS:= -I$(INCDIR) -I./juve

TARGET:= $(BINDIR)/kudoc

all: $(TARGET)

$(TARGET): $(OBJ) $(CPPOBJ)
	clang++ -o $@ $^ $(FLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.c
	clang -c -o $@ $< $(FLAGS)

$(BINDIR)/%.o: juve/%.cpp
	clang++ -c -o $@ $< $(FLAGS)
