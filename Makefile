# Makefile
# Declaration of variables
CXX = g++

SOURCE_DIR = src
BUILD_DIR = build/debug

HEADER_DIRS = -Iinclude -Iexternal/backward-cpp
# overrides makes it possible to externally append extra flags
override CXXFLAGS += $(HEADER_DIRS) -std=c++14 -Wall -Wextra -Wpedantic -Wpointer-arith -Wconversion -Wshadow
override CXXFLAGS += -Wduplicated-cond -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2
CXXFLAGS_DEBUG = $(CXXFLAGS) -O0 -ggdb

LDFLAGS = -fno-omit-frame-pointer
LDLIBS = -lbfd -ldl

PREFIX = /usr/local

# File names
EXEC = sputil
LIB = lib$(EXEC)
# Recursively search for cpp files
SOURCES = $(shell find $(SOURCE_DIR) -iname "*.cpp" | xargs)
# Translate cpp file names to there corresponding build location
OBJECTS = $(patsubst $(SOURCE_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
# Translate the object file names to depend file names
DEPENDS = $(OBJECTS:.o=.d)

# PHONY targets is not file backed targets
.PHONY: test all clean install uninstall bear $(EXEC)

# all {{{
# The "all" target. Runs by default since it the first target
all: ${EXEC}
	$(AR) rcs $(BUILD_DIR)/$(LIB).a $(OBJECTS)
# }}}

# $(EXEC) {{{
# depends on the targets for all the object files
$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $(EXEC)
# }}}

# object {{{

#include the raw object file dependencies from its *.d file
-include $(DEPENDS)

# The "object" file target depends on the corresponding source file
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	mkdir -p $(dir $(@))
# -c means to create an intermediary object file, rather than an executable
# -MMD means to create *object*.d depend file with its depending cpp & h files
	$(CXX) $(CXXFLAGS_DEBUG) -MMD -c $< -o $@
# }}}

# clean {{{
clean: dumb_clean
	$(MAKE) -C test clean

dumb_clean:
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)
	rm -f $(EXEC) $(BUILD_DIR)/*.a $(BUILD_DIR)/*.so
# }}}

# test {{{
test:
	$(MAKE) -C test test
# }}}

# # staticlib {{{
# staticlib: $(OBJECTS)
# # 'r' means to insert with replacement
# # 'c' means to create a new archive
# # 's' means to write an index
# 	$(AR) rcs $(BUILD_DIR)/$(LIB).a $(OBJECTS)
# # }}}

# install {{{
install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/lib
# mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1
	cp -f $(EXEC) $(DESTDIR)$(PREFIX)/bin
	cp -f $(BUILD_DIR)/$(LIB).a $(DESTDIR)$(PREFIX)/lib
# gzip < $(EXEC).1 > $(DESTDIR)$(PREFIX)/share/man/man1/$(EXEC).1.gz
# }}}

# uninstall {{{
uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/$(EXEC)
	rm $(DESTDIR)$(PREFIX)/bin/$(lib).a
# rm $(DESTDIR)$(PREFIX)/share/man/man1/$(EXEC).1.gz
# }}}

# bear {{{
# Creates compilation_database.json
bear:
	$(MAKE) -C test bear
	make BUILD_DIR=build/bear clean
	bear make BUILD_DIR=build/bear CXXFLAGS+=-DSP_TEST -j 4
	compdb list > tmp_compile_commands.json
	mv tmp_compile_commands.json compile_commands.json
# }}}
