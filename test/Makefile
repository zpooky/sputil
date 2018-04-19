# Makefile
# Declaration of variables
CXX = g++
# using -std=gnu++14 because gtest uses functions only presisent in the GNU dialect of c++
PROJECT_ROOT = ..
SUT_DIR = build/test

SOURCE_DIR=.

GTEST_ROOT = $(PROJECT_ROOT)/external/googletest/googletest

HEADER_DIRS = -I$(GTEST_ROOT)/include -I$(PROJECT_ROOT)/include
override CXXFLAGS += $(HEADER_DIRS) -std=gnu++14 -Wall -Wextra -Wpointer-arith -DSP_TEST
CXXFLAGS_DEBUG = $(CXXFLAGS) -ggdb

LDLIBS = -L$(GTEST_ROOT)/make -lgtest -lpthread -L$(PROJECT_ROOT)/$(SUT_DIR) -lsut
LDFLAGS = -fno-omit-frame-pointer

BUILD_DIR = build

# File names
EXEC = thetest
SOURCES = $(shell find $(SOURCE_DIR) -iname "*.cpp" | xargs)
OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
DEPENDS = $(OBJECTS:.o=.d)

.PHONY: test all clean gtest sut bear $(EXEC)

# all {{{
all: sut $(EXEC)
# }}}

# $(EXEC) linking {{{
$(EXEC): $(GTEST_ROOT)/make/gtest.a $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $(EXEC)
# }}}

# compile {{{
-include $(DEPENDS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $(@))
	$(CXX) $(CXXFLAGS_DEBUG) -MMD -c $< -o $@
# }}}

# clean {{{
clean:
# clean test binaries
	rm -f $(EXEC) $(OBJECTS) $(DEPENDS)
	$(MAKE) -C $(GTEST_ROOT)/make clean
# Clean sut binaries
	$(MAKE) BUILD_DIR=$(SUT_DIR) -C $(PROJECT_ROOT) dumb_clean
# }}}

# gtest {{{
LIBGTEST_ROOT = $(GTEST_ROOT)/make
$(GTEST_ROOT)/make/gtest.a:
	$(MAKE) -C $(GTEST_ROOT)/make gtest.a -j
	cp $(LIBGTEST_ROOT)/gtest.a $(LIBGTEST_ROOT)/libgtest.a
# }}}

# sut {{{
sut:
# Build sut binaries and put result in $(SUT_DIR) directory to differentiate the build for test binaries with regular binaries.
# Define the SP_TEST preprocessor macro
# The resulting static library will be called libsut
	$(MAKE) BUILD_DIR=$(SUT_DIR) LIB=libsut CXXFLAGS+=-DSP_TEST -C $(PROJECT_ROOT) -j
# }}}

# bear {{{
# Creates compilation_database.json
bear:
	make BUILD_DIR=build/bear clean
	bear make BUILD_DIR=build/bear CXXFLAGS+=-DSP_TEST -j 4
	compdb list > tmp_compile_commands.json
	mv tmp_compile_commands.json compile_commands.json
# }}}

# test {{{
test: all
	./$(EXEC)
# }}}
