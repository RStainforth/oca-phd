# Makefile for the original OCA (SNO Version Libraries)
# NOTE: The implementation of these libraries here are used
# only to supplement the features of OCA. They are not intended for single
# use themselves
#
# Instructions: make clean, make, make install

# Compile and link flags imported from ROOT 
ROOTCFLAGS  := $(shell root-config --cflags)
ROOTLIBS    :=  $(shell root-config --libs) -lMinuit

# Shouldn't need to following. For some strange reason libMinuit isn't part of the linking libraries
ROOTGLIBS   :=  $(shell root-config --glibs) -lMinuit

# Defines the archictecture of your machine. This is used to decide the linking command
ARCH	    := $(shell root-config --arch)

# This flag in ROOT is only specified for the architecture. Use it to make sure that consistenty is kept
BUILD		:=  $(shell root-config --ldflags)
LDOPTS		:= 
LINKCMD		:=

ifneq ($(findstring macosx, $(ARCH)),)
	LDOPTS := -dynamiclib -single_module -undefined dynamic_lookup
	LINKCMD := install_name_tool -change lib/libRATEvent_$(RATSYSTEM).so @rpath/libRATEvent_$(RATSYSTEM).so libOCA_SNO.so
endif
ifneq ($(findstring $(ARCH), $(strip linux debian redhat)),)
	LDOPTS := -shared
endif

# If LDOPTS is still empty at this point use the linux flag
ifeq ($(strip $(LDOPTS)),)
	LDOPTS := -shared
endif

### Some options for this package
OCA_SNO_VERSION:= "1.0.0" # The LOCAS version
OCA_SNOPLUS_COMPATIBILITY:= "1.0.0" 
OCA_SNO_ROOT 	:= $(OCA_SNOPLUS_ROOT)
OCA_SNO_LIB	:= $(OCA_SNO_ROOT)/lib 
OCA_SNO_SRC	:= ./
OCA_SNO_INC	:= $(OCA_SNO_ROOT)/include
RAT_INC		:= -I$(RATROOT)/include -I$(RATROOT)/include/RAT -I$(RATROOT)/include/RAT/DS -I$(RATROOT)/include/RAT/DU
# The name of the library depends on the value of RATSYSTEM, which must be setup to link against rat anyway.
RAT_LIB		:= -L$(RATROOT)/lib -lRATEvent_$(RATSYSTEM)

# Dictionary
DICT_H	:= OCA_SNO_LinkDef.h
DICT_C	:= OCA_SNO_Dict.C
DICT_O	:= $(patsubst %.C,%.o,$(DICT_C))


# N. Barros - 22-11-2011 : Removed from the targets as the source file seems to be missing. Maybe a missing commit?
# sjmptest

OCA_SNO_H		:= $(filter-out $(DICT_H) $(patsubst %.C,%.h,$(DICT_C)),$(notdir $(shell find $(OCA_SNO_SRC) -name "*.h")))
OCA_SNO_C		:= $(notdir $(shell find $(OCA_SNO_SRC) -name "*.cxx"))

OCA_SNO_OBJS	:= $(patsubst %.cxx,%.o,$(OCA_SNO_C))
OCA_SNO_L		:= libOCA_SNO.so


OCA_SNO_CFLAGS	:= -I./
PICFLAG	:= -fPIC

### Below are the building commands. Don't touch if you don't know what you're doing.

.PHONY: setup all install_libraries

all: show headers libraries

install: install_libraries install_headers

show:
	@echo "Build options:"
	@echo " * ARCH  	: $(ARCH)"
	@echo " * LDOPT 	: $(LDOPTS)"
	@echo " * CXX 		: $(CXX)"	

dirs: 
	@mkdir -p $(OCA_SNO_LIB)
	@mkdir -p $(OCA_SNO_INC)

headers: $(OCA_SNO_H)
	@echo " == Executing OCA-SNO headers =="

install_headers: headers
		@echo " == Executing OCA-SNO install_headers =="
		@echo "copying OCA-SNO headers to $(OCA_SNO_INC)"
		@cp $(OCA_SNO_H) $(OCA_SNO_INC)
		@echo "Done."

libraries: $(OCA_SNO_L)
		@echo " == Executing OCA-SNO libraries =="

install_libraries:	libraries dirs
		@echo " == Executing OCA-SNO install_libraries =="
		@echo "copying $(OCA_SNO_L) to $(OCA_SNO_LIB)"
		@cp $(OCA_SNO_L) $(OCA_SNO_LIB)
		@echo "Done."

# Clean should only work on the source dir
# Whatever is on lib, bin and include should be kept there
clean:
	@echo " == Executing OCA-SNO clean =="
	@rm -f *Dict.C
	@rm -f *Dict.h
	@rm -f *.o
	@rm -f *.so
	@rm -f $(LOCAS_EXE)
	@rm -f qu_sno.log
	@echo "Done."

$(OCA_SNO_L): $(OCA_SNO_OBJS) $(DICT_O)
	@echo " == Executing OCA-SNO OCA_SNO_L =="
	@echo "linking $(OCA_SNO_L)"
	$(CXX) $(LDFLAGS) $(PICFLAG) $(LDOPTS) \
	-o $@ $(OCA_SNO_OBJS) $(ROOTLIBS) $(RAT_LIB) $(DICT_O)
	$(LINKCMD)

$(DICT_C): $(DICT_H) $(OCA_SNO_OBJS) $(OCA_SNO_H)
	@echo " == Executing OCA-SNO DICT_C =="
	@rm -f $@
	@echo "Generating $@ dictionary"
	rootcint -f $@ $(ROOTCINTFLAGS) -c $(OCA_SNO_H) $<
	@echo "Done with $@ ..."

%.o:	%.cxx %.h
#	@echo Special rule for .cxx files with headers
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(OCA_SNO_CFLAGS) $(PICFLAG) $(RAT_INC) $(INCLUDE) -DCOMPILED_ROOT -c $<

%.o:	%.C %.h
#	@echo Special rule for .C files with headers
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(OCA_SNO_CFLAGS) $(PICFLAG) $(RAT_INC) $(INCLUDE) -DCOMPILED_ROOT -c $<

%.o:	%.cxx
#	@echo Rule for .cxx files
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(OCA_SNO_CFLAGS) $(PICFLAG) $(RAT_INC) $(INCLUDE) -DCOMPILED_ROOT -c $<

%.o:	%.C
#	@echo Rule for .C files
	$(CXX) $(CXXFLAGS) $(ROOTCFLAGS) $(OCA_SNO_CFLAGS) $(CFLAGS) $(PICFLAG)  $(RAT_INC) $(INCLUDE) -DCOMPILED_ROOT -c $<
