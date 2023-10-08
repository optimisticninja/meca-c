SRC = src
BIN = bin
OBJ = build

C_SOURCES := $(wildcard $(SRC)/*.c)
C_OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(C_SOURCES))

CFLAGS := -Wall -Werror -Wextra -O2 -g
# TODO: Add parallelization
CFLAGS += -fopenmp

MECA_OPTS := -r 0xDEADBEEFCAFEBABE -e 22

.PHONY: build clean

build: format $(BIN)/meca

format:
	@echo Formatting
	@./scripts/format.sh

# http://www.gnu.org/software/make/manual/make.html#Static-Pattern
$(C_OBJECTS): $(OBJ)/%.o: $(SRC)/%.c
	@echo Compiling $<
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BIN)/meca: $(C_OBJECTS)
	@echo Linking $@
	@mkdir -p $(@D)
	@$(CC) $^ $(LDFLAGS) -o $@

args:
	@echo Testing CLI args for input/output files
	@./scripts/test-args.sh

pipe: # TODO
	@echo Test pipe for input

stdout: # TODO
	@echo Testing stdout for redirection

test: args pipe stdout

clean:
	rm -rfv $(BIN) $(OBJ)
