PROG := emu8
TEST := emu8_test

CWD := $(shell pwd)
SRCDIR := src
TESTDIR := test

CXX := g++
CPPFLAGS := -MMD -MP

INCLUDE := -I$(SRCDIR) -I$(TESTDIR)

CXXEXTRA := -g3 -fsanitize=address
#CXXEXTRA := -O3 -flto

CXXFLAGS := $(CXXEXTRA) $(INCLUDE) -std=c++17 -Wall -Wextra -Werror -pedantic -Weffc++
CXXFLAGS += -Wcast-align -Wcast-qual -Wconversion -Wdisabled-optimization
CXXFLAGS += -Wfloat-equal -Wformat=2 -Wformat-nonliteral -pedantic-errors 
CXXFLAGS += -Wformat-security -Wformat-y2k -Wimport -Winit-self
CXXFLAGS += -Winline -Winvalid-pch -Wlong-long -Wmissing-field-initializers
CXXFLAGS += -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn
CXXFLAGS += -Wpacked -Wpointer-arith -Wredundant-decls -Wshadow
CXXFLAGS += -Wstack-protector -Wstrict-aliasing=2 -Wswitch-default -Wswitch-enum
CXXFLAGS += -Wunreachable-code -Wunused -Wunused-parameter -Wvariadic-macros
CXXFLAGS += -Wwrite-strings

LDEXTRA := -fsanitize=address
#LDEXTRA := -flto

LDFLAGS := $(LDEXTRA) -lSDL2

BUILD := build
BIN := bin

SRCLIST := $(shell ls $(SRCDIR)/*.cpp)
OBJLIST := $(SRCLIST:$(SRCDIR)/%.cpp=$(BUILD)/%.o)

TESTSRC := $(shell ls $(TESTDIR)/*.cpp)
TESTOBJ := $(TESTSRC:$(TESTDIR)/%.cpp=$(BUILD)/%.o)

DEPS := $(OBJLIST:%.o=%.d)
DEPS += $(TESTOBJ:%.o=%.d)

.PHONY: clean check test

$(BIN)/$(PROG): $(OBJLIST) | $(BIN)
	$(CXX) $(OBJLIST) -o $@ $(LDFLAGS)

$(BUILD)/%.o: $(SRCDIR)/%.cpp  | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

$(BIN):
	mkdir -p $(BIN)

$(BUILD)/%.o: $(TESTDIR)/%.cpp  | $(BUILD)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BIN)/$(TEST): $(filter-out $(BUILD)/main.o,$(OBJLIST)) $(TESTOBJ) | $(BIN)
	$(CXX) $^ -o $@ $(LDFLAGS)

test: $(BIN)/$(TEST)
	$(CWD)/$(BIN)/$(TEST)

check:
	clang-tidy $(SRCDIR)/* $(TESTDIR)/* -extra-arg=-std=c++17 -extra-arg-before=-xc++ -- $(INCLUDE)

clean:
	rm -r $(BUILD) $(BIN)

-include $(DEPS)
