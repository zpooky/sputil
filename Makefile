# Makefile
CXX = g++
HEADER_DIRS = -Iinclude -Iexternal/backward-cpp
# ovrrides makes it possible to externaly append extra flags
override CXXFLAGS += $(HEADER_DIRS) -std=c++14 -Wall -Wextra -Wpedantic -Wpointer-arith -Wconversion -Wshadow
# requires gcc 7: -Wduplicated-branches -Wrestrict
override CXXFLAGS += -Wduplicated-cond -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion -Wformat=2
override CXXFLAGS += -Wreturn-type -Wcast-align -Wcast-qual -Wuninitialized -Winit-self -Wmissing-include-dirs -Woverloaded-virtual
# -fsanitize=address -fsanitize=thread -fsanitize=undefined -fstack-protector-all -fstack-check
# -g includes DWARF debug info into the result ELF file, will not impact codegen
CXXFLAGS_DEBUG = $(CXXFLAGS) -O0 -ggdb
CXXFLAGS_PROFILE = $(CXXFLAGS_DEBUG) -pg
CXXFLAGS_O1 = $(CXXFLAGS) -O1 -ggdb
CXXFLAGS_O2 = $(CXXFLAGS) -O2 -ggdb
CXXFLAGS_O3 = $(CXXFLAGS) -O3 -ggdb

LDFLAGS = -fno-omit-frame-pointer
LDLIBS = -lbfd -ldl
PREFIX = /usr/local

SOURCE_DIR = src
BUILD_DIR = build/debug

# File names
EXEC = sputil
LIB = lib$(EXEC)
# Recursively search for cpp files
SOURCES = $(shell find $(SOURCE_DIR) -iname "*.cpp" | xargs)
# Translate cpp file names to there corresponding build location
OBJECTS = $(patsubst $(SOURCE_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
# Translate the object file names to depend file names
DEPENDS = $(OBJECTS:.o=.d)

#TODO dynamic link lib
#TODO https://kristerw.blogspot.se/2017/09/useful-gcc-warning-options-not-enabled.html
# https://developers.redhat.com/blog/2018/03/21/compiler-and-linker-flags-gcc/

# all {{{
# The "all" target. runs by default since it the first target
.PHONY: all
all: ${EXEC}
# package
	$(AR) rcs $(BUILD_DIR)/$(LIB).a $(OBJECTS)
# }}}

# $(EXEC) {{{
# the linking stage is dependant on that all object files are compiled
# .PHONY: $(EXEC)
$(EXEC): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $(BUILD_DIR)/$(EXEC)
# }}}

# staticlib {{{
# staticlib: $(OBJECTS)
# 'r' means to insert with replacement
# 'c' means to create a new archive
# 's' means to write an index
# $(AR) rcs $(BUILD_DIR)/$(LIB).a $(OBJECTS)
# }}}

# object {{{

#include the raw object file dependencies from its *.d file
-include $(DEPENDS)

# The "object" file target
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	mkdir -p $(dir $(@))
# -c means to create an intermediary object file, rather than an executable
# -MMD means to create *object*.d depend file with its depending cpp & h files
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@
# }}}

# clean {{{
.PHONY: clean
clean: dumb_clean
	$(MAKE) -C test clean

.PHONY: dumb_clean
dumb_clean:
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)
	rm -f $(BUILD_DIR)/$(EXEC) $(BUILD_DIR)/*.a $(BUILD_DIR)/*.so
# }}}

# - {{{
.PHONY: spooky
spooky:
	$(MAKE) CXXFLAGS="$(CXXFLAGS_DEBUG)"

.PHONY: buildO1
buildO1:
	$(MAKE) BUILD_DIR=build/buildO1 CXXFLAGS="$(CXXFLAGS_O1)"

.PHONY: buildO2
buildO2:
	$(MAKE) BUILD_DIR=build/buildO2 CXXFLAGS="$(CXXFLAGS_O2)"

.PHONY: buildO3
buildO3:
	$(MAKE) BUILD_DIR=build/buildO3 CXXFLAGS="$(CXXFLAGS_O3)"
# }}}

# test {{{
.PHONY: test
test:
	$(MAKE) -C test test
# }}}

# install {{{
.PHONY: install
install: all
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/lib
# mkdir -p $(DESTDIR)$(PREFIX)/share/man/man1
	cp -f $(BUILD_DIR)/$(EXEC) $(DESTDIR)$(PREFIX)/bin
	cp -f $(BUILD_DIR)/$(LIB).a $(DESTDIR)$(PREFIX)/lib
# gzip < $(EXEC).1 > $(DESTDIR)$(PREFIX)/share/man/man1/$(EXEC).1.gz
# }}}

# uninstall {{{
.PHONY: uninstall
uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/$(EXEC)
	rm $(DESTDIR)$(PREFIX)/bin/$(lib).a
# rm $(DESTDIR)$(PREFIX)/share/man/man1/$(EXEC).1.gz
# }}}

# bear {{{
# Creates compilation database
.PHONY: bear
bear:
	$(MAKE) -C test bear
	$(MAKE) BUILD_DIR=build/bear dumb_clean
	bear make BUILD_DIR=build/bear CXXFLAGS+=-DSP_TEST -j
	compdb list > tmp_compile_commands.json
	mv tmp_compile_commands.json compile_commands.json
# }}}

# valgrind {{{
.PHONY: valgrind
valgrind:
	$(MAKE) CXXFLAGS=$(CXXFLAGS_DEBUG) BUILD_DIR=build/valgrind
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./build/valgrind/$(EXEC)
# }}}

# gprof {{{
# only for gcc?
.PHONY: gprof
gprof:
	$(MAKE) CXXFLAGS=$(CXXFLAGS_PROFILE) BUILD_DIR=build/gprof
	./build/gprof/$(EXEC)
# print the callgraph
	gprof cpuload
# }}}
#
