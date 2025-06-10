
SRC := cog.c $(wildcard src/*.c)
BUILD :=build
SRCDIR :=src
OBJ := $(patsubst $(SRCDIR)/%.c, $(BUILD)/%.o, $(SRC))
FLAGS := -Wall -pedantic -Iincludes -g
all: $(BUILD)/cog

$(BUILD)/cog: $(OBJ)
	gcc -o $@ $^ -Iincludes

$(BUILD)/%.o: $(SRCDIR)/%.c
	gcc -c $< -o $@ $(FLAGS)
