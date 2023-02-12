PROG := emu8
CXX := g++

CPPFLAGS := -MMD -MP

CXXEXTRA := -g3 -fsanitize=address
#CXXEXTRA := -O3 -flto

CXXFLAGS := $(CXXEXTRA) -Wall -Wextra -Werror -pedantic -pedantic-errors -Weffc++
CXXFLAGS += -Wcast-align -Wcast-qual -Wconversion -Wdisabled-optimization
CXXFLAGS += -Wfloat-equal -Wformat=2 -Wformat-nonliteral
CXXFLAGS += -Wformat-security -Wformat-y2k -Wimport -Winit-self
CXXFLAGS += -Winline -Winvalid-pch -Wlong-long -Wmissing-field-initializers
CXXFLAGS += -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn
CXXFLAGS += -Wpacked -Wpadded -Wpointer-arith -Wredundant-decls -Wshadow
CXXFLAGS += -Wstack-protector -Wstrict-aliasing=2 -Wswitch-default -Wswitch-enum
CXXFLAGS += -Wunreachable-code -Wunused -Wunused-parameter -Wvariadic-macros
CXXFLAGS += -Wwrite-strings

LDEXTRA := -fsanitize=address
#LDEXTRA := -flto

LDFLAGS := $(LDEXTRA) -lSDL2

SRCDIR := src
BUILD := build
BIN := bin

SRCLIST := $(shell ls $(SRCDIR)/*.cpp)
OBJLIST := $(SRCLIST:$(SRCDIR)/%.cpp=$(BUILD)/%.o)

DEPS := $(OBJLIST:%.o=%.d)

.PHONY: clean

$(BIN)/$(PROG): $(OBJLIST) | $(BIN)
	$(CXX) $(OBJLIST) -o $@ $(LDFLAGS)

$(BUILD)/%.o: $(SRCDIR)/%.cpp  | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

$(BIN):
	mkdir -p $(BIN)

clean:
	rm -r $(BUILD) $(BIN)

-include $(DEPS)
