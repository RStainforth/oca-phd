# Makefile for  $(QOCAL)
# Quick and dirty implementation to build the QOCA module in a standalone way.
#
# There are some issues with the Makefile. The original QSNO code depended on ROOT source tree
# which is not necessary. Therefore the Makefile was refactored to not have these dependencies.
#
# Do a 'make clean' after modifications to this makefile
#
# If for any reason it does not work for you feel free to work on it's improvement
#
# The package now depends solely on ROOT. Therefore it tried to get as
# much information from it as possible through 'root-config'

# Compilationa and link flags imported from ROOT 
ROOTCFLAGS  := $(shell root-config --cflags)
ROOTLIBS    := $(shell root-config --libs) -lMinuit
# Shouldn't need to following. For some strange reason libMinuit isn't part of the linking libraries
ROOTGLIBS   := $(shell root-config --glibs) -lMinuit

# Defines the arch. Is used to decide the linking command
ARCH	    := $(shell root-config --arch)

# This flag in ROOT only specified the architecture. Use it to make sure that consistenty is kept
BUILD		:=  $(shell root-config --ldflags)
LDOPTS		:= 


ifneq ($(findstring macosx, $(ARCH)),)
	LDOPTS := -dynamiclib -single_module -undefined dynamic_lookup
endif
ifneq ($(findstring $(ARCH), $(strip linux debian redhat)),)
	LDOPTS := -shared
endif
# If LDOPTS is still empty at this point use the linux flag
ifeq ($(strip $(LDOPTS)),)
	LDOPTS := -shared
endif

### Some options for this package
QOCA_ROOT 	:= $(QOCA_ROOT)
QOCA_LIB	:= ../lib
QOCA_SRC	:= ./
QOCA_INC	:= ../include
QOCA_BIN	:= ../bin


# Dictionary
DICT_H	:= QOCA_LinkDef.h
DICT_C	:= QOCA_Dict.C
DICT_O	:= $(patsubst %.C,%.o,$(DICT_C))


QOCA_EXE	:= 	qoca_positionfit reflecttime qpath_runner \
				qocafit_runner qocaextract_runner qocaplots_runner \
				qocatitles_runner tdiff

QOCAH		:= $(filter-out $(DICT_H) $(patsubst %.C,%.h,$(DICT_C)),$(notdir $(shell find $(QOCA_SRC) -name "*.h")))
QOCAC		:= $(filter-out $(addsuffix .cxx,$(QOCA_EXE)),$(notdir $(shell find $(QOCA_SRC) -name "*.cxx")))

QOCAOBJS	:= $(patsubst %.cxx,%.o,$(QOCAC))
QOCAL		:= libqoca.so


QOCACFLAGS	:= -I./


PICFLAG	:= -fPIC


### Below are the building commands. Don't touch if you don't know what you're doing.

.PHONY: setup all install_libraries executables install_executables


all: show headers libraries executables
#qoca_positionfit tdiff reflecttime qpath_runner qocafit_runner qocaextract_runner qocaplots_runner qocatitles_runner

install: install_libraries install_executables install_headers

show:
	@echo "Build options:"
	@echo " * ARCH  	: $(ARCH)"
	@echo " * LDOPT 	: $(LDOPTS)"
	@echo " * CXX 		: $(CXX)"
#	@echo " * HEADERS	: $(QOCAH)"
#	@echo " * SOURCES	: $(QOCAC)"
#	@echo " * OBJS		: $(QOCAOBJS)"	

dirs: 
	@mkdir -p $(QOCA_LIB)
	@mkdir -p $(QOCA_BIN)
	@mkdir -p $(QOCA_INC)


headers: $(QOCAH)
	@echo " == Executing $@ =="

install_headers: headers
		@echo " == Executing $@ =="
		@echo "copying QOCA headers to $(QOCA_INC)"
		@cp $(QOCAH) $(QOCA_INC)
		@echo "Done."

libraries: $(QOCAL)
		@echo " == Executing $@ =="

install_libraries:	libraries dirs
		@echo " == Executing $@ =="
		@echo "copying $(QOCAL) to $(QOCA_LIB)"
		@cp $(QOCAL) $(QOCA_LIB)
		@echo "Done."

executables: $(QOCA_EXE)
		@echo " == Executing $@ =="

install_executables:	executables dirs
		@echo " == Executing $@ =="
		@echo "copying QOCA executables to $(QOCA_BIN)"
		@cd $(QOCA_SRC)
		@cp $(QOCA_EXE) $(QOCA_ROOT)/bin
		@cd $(QOCA_ROOT)
		@echo "Done"


# Clean should only work on the source dir
# Whatever is on lib, bin and include should be kept there
clean:
	@echo " == Executing $@ =="
	@rm -f *Dict.C
	@rm -f *Dict.h
	@rm -f *.o
	@rm -f *.so
	@rm -f $(QOCA_EXE)
	@rm -f qu_sno.log
	@echo "Done."

$(QOCAL):	$(QOCAOBJS) $(DICT_O)
	@echo " == Executing $@ =="
	@echo "linking $(QOCAL)"
	$(CXX) $(LDFLAGS) $(PICFLAG) $(LDOPTS) \
	-o $@ $(QOCAOBJS) $(ROOTLIBS) $(DICT_O)

qoca_positionfit: qoca_positionfit.o $(QOCAL) 
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$<  $(ROOTLIBS) -L./ -lqoca 

tdiff:	tdiff.o  $(QOCAL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$< $(ROOTLIBS) -L. -lqoca 

reflecttime:	reflecttime.o  $(QOCAL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$<  $(ROOTLIBS) -L. -lqoca 

qpath_runner: qpath_runner.o $(QOCAL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$<  $(ROOTLIBS) -L. -lqoca

qocafit_runner: qocafit_runner.o $(QOCAL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$< $(ROOTLIBS) -L. -lqoca 

qocaextract_runner: qocaextract_runner.o $(QOCAL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$<  $(ROOTLIBS) -L. -lqoca

qocaplots_runner: qocaplots_runner.o $(QOCAL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$<  $(ROOTLIBS) -L. -lqoca

qocatitles_runner: qocatitles_runner.o $(QOCAL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$<  $(ROOTLIBS) -L. -lqoca

$(DICT_C): $(DICT_H) $(QOCAO) 
	@echo " == Executing $@ =="
	@rm -f $@
	@echo "Generating $@ dictionary"
	rootcint -f $@ $(ROOTCINTFLAGS) -c $(QOCAH) $<
	@echo "Done with $@ ..."

%.o:	%.cxx %.h
#	@echo Special rule for .cxx files with headers
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(QOCACFLAGS) $(PICFLAG) $(INCLUDE) -DCOMPILED_ROOT -c $<

%.o:	%.C %.h
#	@echo Special rule for .C files with headers
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(QOCACFLAGS) $(PICFLAG) $(INCLUDE) -DCOMPILED_ROOT -c $<

%.o:	%.cxx
#	@echo Rule for .cxx files
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(QOCACFLAGS) $(PICFLAG) $(INCLUDE) -DCOMPILED_ROOT -c $<
%.o:	%.C

#	@echo Rule for .C files
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(QOCACFLAGS) $(PICFLAG) $(INCLUDE) -DCOMPILED_ROOT -c $<
