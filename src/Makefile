# Makefile for  $(LOCAS)
# Quick and dirty implementation to build the LOCAS module in a standalone way.
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
LOCAS_ROOT 	:= $(LOCAS_ROOT)
LOCAS_LIB	:= ../lib 
LOCAS_SRC	:= ./
LOCAS_INC	:= ../include 
LOCAS_BIN	:= ../bin
RAT_INC		:= -I$(RATROOT)/include -I$(RATROOT)/include/RAT 
# The name of the library depends on the value of G4SYSTEM, which must be setup to link agains rat anyway.
RAT_LIB		:= -L$(RATROOT)/lib -lRATEvent_$(G4SYSTEM)

# Dictionary
DICT_H	:= LOCAS_LinkDef.h
DICT_C	:= LOCAS_Dict.C
DICT_O	:= $(patsubst %.C,%.o,$(DICT_C))


LOCAS_EXE	:= 	locas_positionfit reflecttime qpath_runner \
				locasfit_runner locasextract_runner locasplots_runner \
				locastitles_runner tdiff 
# N. Barros - 22-11-2011 : Removed from the targets as the source file seems to be missing. Maybe a missing commit?
# sjmptest

LOCASH		:= $(filter-out $(DICT_H) $(patsubst %.C,%.h,$(DICT_C)),$(notdir $(shell find $(LOCAS_SRC) -name "*.h")))
LOCASC		:= $(filter-out $(addsuffix .cxx,$(LOCAS_EXE)),$(notdir $(shell find $(LOCAS_SRC) -name "*.cxx")))

LOCASOBJS	:= $(patsubst %.cxx,%.o,$(LOCASC))
LOCASL		:= libLOCAS.so


LOCASCFLAGS	:= -I./


PICFLAG	:= -fPIC


### Below are the building commands. Don't touch if you don't know what you're doing.

.PHONY: setup all install_libraries executables install_executables


all: show headers libraries executables
#LOCAS_positionfit tdiff reflecttime qpath_runner LOCASfit_runner LOCASextract_runner LOCASplots_runner LOCAStitles_runner

install: install_libraries install_executables install_headers

show:
	@echo "Build options:"
	@echo " * ARCH  	: $(ARCH)"
	@echo " * LDOPT 	: $(LDOPTS)"
	@echo " * CXX 		: $(CXX)"
#	@echo " * HEADERS	: $(LOCASH)"
#	@echo " * SOURCES	: $(LOCASC)"
#	@echo " * OBJS		: $(LOCASOBJS)"	

dirs: 
	@mkdir -p $(LOCAS_LIB)
	@mkdir -p $(LOCAS_BIN)
	@mkdir -p $(LOCAS_INC)


headers: $(LOCASH)
	@echo " == Executing $@ =="

install_headers: headers
		@echo " == Executing $@ =="
		@echo "copying LOCAS headers to $(LOCAS_INC)"
		@cp $(LOCASH) $(LOCAS_INC)
		@echo "Done."

libraries: $(LOCASL)
		@echo " == Executing $@ =="

install_libraries:	libraries dirs
		@echo " == Executing $@ =="
		@echo "copying $(LOCASL) to $(LOCAS_LIB)"
		@cp $(LOCASL) $(LOCAS_LIB)
		@echo "Done."

executables: $(LOCAS_EXE)
		@echo " == Executing $@ =="

install_executables:	executables dirs
		@echo " == Executing $@ =="
		@echo "copying LOCAS executables to $(LOCAS_BIN)"
		@cd $(LOCAS_SRC)
		@cp $(LOCAS_EXE) $(LOCAS_ROOT)/bin
		@cd $(LOCAS_ROOT)
		@echo "Done"


# Clean should only work on the source dir
# Whatever is on lib, bin and include should be kept there
clean:
	@echo " == Executing $@ =="
	@rm -f *Dict.C
	@rm -f *Dict.h
	@rm -f *.o
	@rm -f *.so
	@rm -f $(LOCAS_EXE)
	@rm -f qu_sno.log
	@echo "Done."

reallyclean: clean
	@echo " == Doing additional cleaning tasks ( $@ ) =="
	@rm -f $(LOCAS_ROOT)/bin/*
	@rm -f $(LOCAS_ROOT)/include/*
	@rm -f $(LOCAS_ROOT)/lib/*
	@echo "Done."

$(LOCASL):	$(LOCASOBJS) $(DICT_O)
	@echo " == Executing $@ =="
	@echo "linking $(LOCASL)"
	$(CXX) $(LDFLAGS) $(PICFLAG) $(LDOPTS) \
	-o $@ $(LOCASOBJS) $(ROOTLIBS) $(RAT_LIB) $(DICT_O)

locas_positionfit: locas_positionfit.o $(LOCASL) 
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$<  $(ROOTLIBS) -L. -lLOCAS 

tdiff:	tdiff.o  $(LOCASL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$< $(ROOTLIBS) -L. -lLOCAS 


sjmptest:	sjmptest.o  $(LOCASL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$< $(ROOTLIBS) $(RAT_LIB) -L. -lLOCAS 


reflecttime:	reflecttime.o  $(LOCASL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$<  $(ROOTLIBS) -L. -lLOCAS 

qpath_runner: qpath_runner.o $(LOCASL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$<  $(ROOTLIBS) -L. -lLOCAS

locasfit_runner: locasfit_runner.o $(LOCASL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$< $(ROOTLIBS) -L. -lLOCAS 

locasextract_runner: locasextract_runner.o $(LOCASL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$<  $(ROOTLIBS) -L. -lLOCAS

locasplots_runner: locasplots_runner.o $(LOCASL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$<  $(ROOTLIBS) -L. -lLOCAS

locastitles_runner: locastitles_runner.o $(LOCASL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
		$<  $(ROOTLIBS) -L. -lLOCAS

$(DICT_C): $(DICT_H) $(LOCASO) $(LOCASH)
	@echo " == Executing $@ =="
	@rm -f $@
	@echo "Generating $@ dictionary"
	rootcint -f $@ $(ROOTCINTFLAGS) -c $(LOCASH) $<
	@echo "Done with $@ ..."

%.o:	%.cxx %.h
#	@echo Special rule for .cxx files with headers
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(LOCASCFLAGS) $(PICFLAG) $(RAT_INC) $(INCLUDE) -DCOMPILED_ROOT -c $<

%.o:	%.C %.h
#	@echo Special rule for .C files with headers
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(LOCASCFLAGS) $(PICFLAG) $(RAT_INC) $(INCLUDE) -DCOMPILED_ROOT -c $<

%.o:	%.cxx
#	@echo Rule for .cxx files
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(LOCASCFLAGS) $(PICFLAG) $(RAT_INC) $(INCLUDE) -DCOMPILED_ROOT -c $<
%.o:	%.C

#	@echo Rule for .C files
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(LOCASCFLAGS) $(CFLAGS) $(PICFLAG)  $(RAT_INC) $(INCLUDE) -DCOMPILED_ROOT -c $<
