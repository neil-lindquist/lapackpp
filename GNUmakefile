# Usage:
# make by default:
#    - Runs configure.py to create make.inc, if it doesn't exist.
#    - Compiles lib/liblapackpp.so, or lib/liblapackpp.a (if static=1).
#    - Compiles the tester, test/test.
#
# make config    - Runs configure.py to create make.inc.
# make lib       - Compiles lib/liblapackpp.so, or liblapackpp.a (if static=1).
# make test      - Compiles the tester, test/test.
# make docs      - Compiles Doxygen documentation.
# make install   - Installs the library and headers to $prefix.
# make clean     - Deletes all objects, libraries, and the tester.
# make distclean - Also deletes make.inc and dependency files (*.d).

#-------------------------------------------------------------------------------
# Configuration
# Variables defined in make.inc, or use make's defaults:
#   CXX, CXXFLAGS   -- C compiler and flags
#   LDFLAGS, LIBS   -- Linker options, library paths, and libraries
#   AR, RANLIB      -- Archiver, ranlib updates library TOC
#   prefix          -- where to install LAPACK++

include make.inc

# Existence of .make.inc.$${PPID} is used so 'make config' doesn't run
# configure.py twice when make.inc doesn't exist initially.
make.inc:
	python configure.py
	touch .make.inc.$${PPID}

.PHONY: config
config:
	if [ ! -e .make.inc.$${PPID} ]; then \
		python configure.py; \
	fi

# defaults if not given in make.inc
CXXFLAGS ?= -O3 -std=c++11 -MMD \
            -Wall -pedantic \
            -Wshadow \
            -Wno-unused-local-typedefs \
            -Wno-unused-function \

#CXXFLAGS += -Wmissing-declarations
#CXXFLAGS += -Wconversion
#CXXFLAGS += -Werror

# GNU make doesn't have defaults for these
RANLIB   ?= ranlib
prefix   ?= /usr/local/lapackpp

# auto-detect OS
# $OSTYPE may not be exported from the shell, so echo it
ostype = $(shell echo $${OSTYPE})
ifneq ($(findstring darwin, $(ostype)),)
	# MacOS is darwin
	macos = 1
endif

#-------------------------------------------------------------------------------
# if shared
ifneq ($(static),1)
	CXXFLAGS += -fPIC
	LDFLAGS  += -fPIC
endif

#-------------------------------------------------------------------------------
# MacOS needs shared library's path set
ifeq ($(macos),1)
	install_name = -install_name @rpath/$(notdir $@)
else
	install_name =
endif

#-------------------------------------------------------------------------------
# Files

lib_src  = $(wildcard src/*.cc)
lib_obj  = $(addsuffix .o, $(basename $(lib_src)))
dep      = $(addsuffix .d, $(basename $(lib_src)))

test_src = $(wildcard test/*.cc)
test_obj = $(addsuffix .o, $(basename $(test_src)))
dep     += $(addsuffix .d, $(basename $(test_src)))

test     = test/test

libtest_dir = ../libtest
libtest_src = $(wildcard $(libtest_dir)/*.cc $(libtest_dir)/*.hh)
libtest     = $(libtest_dir)/libtest.so

blaspp_dir = ../blaspp
blaspp_src = $(wildcard $(blaspp_dir)/src/*.cc $(blaspp_dir)/include/*.hh)
#blaspp    = $(blaspp_dir)/lib/libblaspp.so  # todo

lib_a  = ./lib/liblapackpp.a
lib_so = ./lib/liblapackpp.so

ifeq ($(static),1)
	lib = $(lib_a)
else
	lib = $(lib_so)
endif

#-------------------------------------------------------------------------------
# LAPACK++ specific flags and libraries
CXXFLAGS += -I./include
CXXFLAGS += -I$(blaspp_dir)/include

# additional flags and libraries for testers
$(test_obj): CXXFLAGS += -I$(blaspp_dir)/test    # for blas_flops.hh
$(test_obj): CXXFLAGS += -I$(libtest_dir)

TEST_LDFLAGS += -L./lib -Wl,-rpath,$(abspath ./lib)
TEST_LDFLAGS += -L$(libtest_dir) -Wl,-rpath,$(abspath $(libtest_dir))
TEST_LIBS    += -llapackpp -ltest

#-------------------------------------------------------------------------------
# Rules
.DELETE_ON_ERROR:
.SUFFIXES:
.PHONY: all lib src test headers include docs clean distclean
.DEFAULT_GOAL = all

all: lib test

install: lib
	mkdir -p $(DESTDIR)$(prefix)/include
	mkdir -p $(DESTDIR)$(prefix)/lib$(LIB_SUFFIX)
	cp include/*.{h,hh} $(DESTDIR)$(prefix)/include
	cp lib/liblapackpp.* $(DESTDIR)$(prefix)/lib$(LIB_SUFFIX)

uninstall:
	$(RM) $(addprefix $(DESTDIR)$(prefix), $(headers))
	$(RM) $(DESTDIR)$(prefix)/lib$(LIB_SUFFIX)/liblapackpp.*

#-------------------------------------------------------------------------------
# liblapackpp library
$(lib_so): $(lib_obj)
	mkdir -p lib
	$(CXX) $(LDFLAGS) -shared $(install_name) $(lib_obj) $(LIBS) -o $@

$(lib_a): $(lib_obj)
	mkdir -p lib
	$(RM) $@
	$(AR) cr $@ $(lib_obj)
	$(RANLIB) $@

# sub-directory rules
lib src: $(lib)

lib/clean src/clean:
	$(RM) lib/*.{a,so} src/*.o

#-------------------------------------------------------------------------------
# tester
$(test): $(test_obj) $(lib) $(libtest)
	$(CXX) $(TEST_LDFLAGS) $(LDFLAGS) $(test_obj) \
		$(TEST_LIBS) $(LIBS) -o $@

# forward libtest to libtest directory
$(libtest): $(libtest_src)
	cd $(libtest_dir) && $(MAKE)

# sub-directory rules
test: $(test)

test/clean:
	$(RM) $(test) test/*.o

#-------------------------------------------------------------------------------
# headers
# precompile headers to verify self-sufficiency
headers     = $(wildcard include/*.h include/*.hh test/*.hh)
headers_gch = $(addsuffix .gch, $(headers))

headers: $(headers_gch)

headers/clean:
	$(RM) include/*.h.gch include/*.hh.gch test/*.hh.gch

# sub-directory rules
include: headers

include/clean: headers/clean

#-------------------------------------------------------------------------------
# documentation
docs:
	doxygen docs/doxygen/doxyfile.conf
	@echo ========================================
	cat docs/doxygen/errors.txt
	@echo ========================================
	@echo "Documentation available in docs/html/index.html"
	@echo ========================================

# sub-directory redirects
src/docs: docs
test/docs: docs

#-------------------------------------------------------------------------------
# general rules
clean: lib/clean test/clean headers/clean

distclean: clean
	$(RM) make.inc src/*.d test/*.d

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# preprocess source
%.i: %.cc
	$(CXX) $(CXXFLAGS) -I$(blaspp_dir)/test -I$(libtest_dir) -E $< -o $@

# precompile header to check for errors
%.h.gch: %.h
	$(CXX) $(CXXFLAGS) -I$(blaspp_dir)/test -I$(libtest_dir) -c $< -o $@

%.hh.gch: %.hh
	$(CXX) $(CXXFLAGS) -I$(blaspp_dir)/test -I$(libtest_dir) -c $< -o $@

-include $(dep)

#-------------------------------------------------------------------------------
# debugging
echo:
	@echo "static        = '$(static)'"
	@echo
	@echo "lib_a         = $(lib_a)"
	@echo "lib_so        = $(lib_so)"
	@echo "lib           = $(lib)"
	@echo
	@echo "lib_src       = $(lib_src)"
	@echo
	@echo "lib_obj       = $(lib_obj)"
	@echo
	@echo "test_src      = $(test_src)"
	@echo
	@echo "test_obj      = $(test_obj)"
	@echo
	@echo "test          = $(test)"
	@echo
	@echo "dep           = $(dep)"
	@echo
	@echo "libtest_dir   = $(libtest_dir)"
	@echo "libtest_src   = $(libtest_src)"
	@echo "libtest       = $(libtest)"
	@echo
	@echo "blaspp_dir    = $(blaspp_dir)"
	@echo "blaspp_src    = $(blaspp_src)"
	@echo "blaspp        = $(blaspp)  # todo"
	@echo
	@echo "CXX           = $(CXX)"
	@echo "CXXFLAGS      = $(CXXFLAGS)"
	@echo
	@echo "LDFLAGS       = $(LDFLAGS)"
	@echo "LIBS          = $(LIBS)"
	@echo
	@echo "TEST_LDFLAGS  = $(TEST_LDFLAGS)"
	@echo "TEST_LIBS     = $(TEST_LIBS)"
