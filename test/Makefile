# Makefile
# Declaration of variables
CXX = g++
# using -std=gnu++14 because gtest uses functions only presisent in the GNU dialect of c++
PROJECT_ROOT = ..
SRC_ROOT = $(PROJECT_ROOT)/src
SUT_DIR = build/test

GTEST_ROOT = $(PROJECT_ROOT)/external/googletest/googletest

HEADER_DIRS = -I$(GTEST_ROOT)/include -I$(SRC_ROOT)
override CXXFLAGS += $(HEADER_DIRS) -enable-frame-pointers -std=gnu++14 -Wall -Wextra -Wpointer-arith -DSP_TEST
CXXFLAGS_DEBUG = $(CXXFLAGS) -ggdb
LDLIBS = -L$(GTEST_ROOT)/make -lgtest_main -lpthread -L$(PROJECT_ROOT)/$(SUT_DIR) -lsut
BUILD_DIR = build

# TODO `make all` get relinks to gtest because I copy gtest_main to lib... and change time stamp?
# TODO coverage
# TODO sut external headers?

# File names
EXEC = thetest
SOURCES = $(shell find $(SOURCE_DIR) -iname *.cpp | xargs)
OBJECTS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
DEPENDS = $(OBJECTS:.o=.d)

.PHONY: test all clean gtest

# all {{{
all: sut $(EXEC)
# }}}

# $(EXEC) linking {{{
$(EXEC): gtest $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXEC) $(LDLIBS)
# }}}

# compile {{{
-include $(DEPENDS)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $(@))
	$(CXX) $(CXXFLAGS_DEBUG) $(LDFLAGS) -MMD -c $< -o $@
# }}}

# clean {{{
clean:
# clean test binaries
	rm -f $(EXEC) $(OBJECTS) $(DEPENDS)
# Clean sut binaries
	$(MAKE) BUILD_DIR=$(SUT_DIR) -C $(PROJECT_ROOT) dumb_clean
# }}}

# gtest {{{
LIBGTEST_ROOT = $(GTEST_ROOT)/make
gtest:
	$(MAKE) -C $(GTEST_ROOT)/make all -j 4
	cp $(LIBGTEST_ROOT)/gtest_main.a $(LIBGTEST_ROOT)/libgtest_main.a
# }}}

# sut {{{
sut:
# Build sut binaries and put result in $(SUT_DIR) directory to differentiate the build for test binaries with regular binaries.
# Define the SP_TEST preprocessor macro
# The resulting static library will be called libsut
	$(MAKE) BUILD_DIR=$(SUT_DIR) LIB=libsut CXXFLAGS+=-DSP_TEST -C $(PROJECT_ROOT)
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
