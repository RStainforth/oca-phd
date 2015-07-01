# Makefile for OCA
#
# Do 'make clean' at the command line after modifications to this makefile
#
# OCA depends solely on ROOT. It therefore tries to get as
# much information from ROOT as possible through 'root-config'

# Compile and link flags imported from ROOT 
ROOTCFLAGS  := $(shell root-config --cflags)
ROOTLIB     := $(shell root-config --libs) -lMinuit

# Compile and link flags imported from GEANT4 
GEANT4CFLAGS  := $(shell geant4-config --cflags)
GEANT4LLIB    := $(shell geant4-config --libs)

# CLHEP Isn't installed by default, so we don't need to use the below just yet.
# Compile and link flags imported from CLHEP 
# CLHEPCFLAGS := $(shell clhep-config --include)
# CLHEPLIB    := $(shell clhep-config --libs)

# Compile and link flags imported from CURL 
CURLCFLAGS  := $(shell curl-config --cflags)
CURLLIB     := $(shell curl-config --libs)

DEPCFLAGS   := ${ROOTCFLAGS} ${GEANT4CFLAGS} ${CLHEPCFLAGS} ${CURLCFLAGS}
DEPLIBS     := ${ROOTLIB} ${GEANT4LIB} ${CLHEPLIB} ${CURLLIB}

# The name of the library depends on the value of RATSYSTEM, which must be setup to link against rat anyway.
# Compile and link flags imported from RAT
RATCFLAGS   := -I$(RATROOT)/include -I$(RATROOT)/include/RAT -I$(RATROOT)/include/RAT/DS
RATLIB		:= -L$(RATROOT)/lib -lRATEvent_$(RATSYSTEM) -lrat_$(RATSYSTEM)

# Compile and link flags for OCA
OCACFLAGS := -I./
OCALIB    := -L. -lOCA

PICFLAG	:= -fPIC

# Defines the archictecture of your machine. This is used to decide the linking command
ARCH	    := $(shell root-config --arch)

# This flag in ROOT is only specified for the architecture. Use it to make sure that consistenty is kept
BUILD		:=  $(shell root-config --ldflags)
LDOPTS		:= 
LINKCMD		:=

ifneq ($(findstring macosx, $(ARCH)),)
	LDOPTS  := -dynamiclib -single_module -undefined dynamic_lookup

# Shouldn't need the below statement anymore - will keep it here for legacy control.
#LINKCMD := install_name_tool -change lib/libRATEvent_$(RATSYSTEM).so @rpath/libRATEvent_$(RATSYSTEM).so libOCA.so
endif
ifneq ($(findstring $(ARCH), $(strip linux debian redhat)),)
	LDOPTS := -shared
endif
# If LDOPTS is still empty at this point use the linux flag
ifeq ($(strip $(LDOPTS)),)
	LDOPTS := -shared
endif

### Some options for this package
OCA_VERSION:= "1.0.0" # The OCA version
OCA_COMPATIBILITY:= "1.0.0" 
OCA_ROOT 	:= $(OCA_ROOT)
OCA_LIB	:= $(OCA_ROOT)/lib 
OCA_SRC	:= $(OCA_ROOT)/src
OCA_INC	:= $(OCA_ROOT)/include
OCA_BIN	:= $(OCA_ROOT)/bin
OCA_OBJ   := $(OCA_ROOT)/obj

# Dictionary
DICT_H	:= OCA_LinkDef.h
DICT_C	:= OCA_Dict.C
DICT_O	:= $(patsubst %.C,%.o,$(DICT_C))

OCA_EXE	:= soc2oca oca2fit oca2marquardt oca2fit2eff makedatastore

# N. Barros - 22-11-2011 : Removed from the targets as the source file seems to be missing. Maybe a missing commit?
# sjmptest

# Creates a list of all of the *.hh files in the current directory (same directory as the Makefile) EXCEPT the filenames
# defined by the values of $DICT_H, and $DICT_C but with the .C extension replaced by .h
# i.e. Not the files OCA_LinkDef.h and OCA_Dict.C
OCAH		:= $(filter-out $(DICT_H) $(patsubst %.C,%.h,$(DICT_C)),$(notdir $(shell find $(OCA_SRC) -name "*.hh")))
# Creates a list of all of the *.cc files in the current directory (same directory as the Makefile) EXCEPT the filenames
# defined by the values of $OCA_EXE, with .cc attached i.e. Not oca.cc
OCAC		:= $(filter-out $(addsuffix .cc,$(OCA_EXE)),$(notdir $(shell find $(OCA_SRC) -name "*.cc")))

# List of the OCA objects, i.e. all those files attached in $OCAC but with the extensions .cc replaced
# with .o
OCAO	    := $(patsubst %.cc,%.o,$(OCAC))
OCAL		:= libOCA.so

### Below are the building commands. Don't touch if you don't know what you're doing.

.PHONY: setup all install_libraries executables install_executables


all: show headers libraries executables
#OCA_positionfit tdiff reflecttime qpath_runner OCAfit_runner OCAextract_runner OCAplots_runner OCAtitles_runner

test:
	@cd $(OCA_ROOT)

dirs: 
	@mkdir -p $(OCA_LIB)
	@mkdir -p $(OCA_BIN)
	@mkdir -p $(OCA_INC)
	@mkdir -p $(OCA_OBJ)

show:
	@echo "Build options:"
	@echo " * ARCH  	: $(ARCH)"
	@echo " * LDOPT 	: $(LDOPTS)"

headers: $(OCAH)
	@echo " == Executing $@ =="	

libraries: $(OCAL)
		@echo " == Executing $@ =="

executables: $(OCA_EXE)
		@echo " == Executing $@ =="

install: install_libraries install_executables install_headers

install_headers: headers
		@echo " == Executing $@ =="
		@echo "copying OCA headers to $(OCA_INC)"
		@cp $(OCAH) $(OCA_INC)
		@echo "Done."

install_libraries: libraries dirs
		@echo " == Executing $@ =="
		@echo "copying $(OCAL) to $(OCA_LIB)"
		@cp $(OCAL) $(OCA_LIB)
		@cp $(OCAO) $(OCA_OBJ)
		@echo "Done."

install_executables: executables dirs
		@echo " == Executing $@ =="
		@echo "copying OCA executables to $(OCA_BIN)"
		@cd $(OCA_SRC)
		@cp $(OCA_EXE) $(OCA_ROOT)/bin
		@cd $(OCA_ROOT)
		@echo "Done"



clean:
	@echo " == Executing $@ =="
	@rm -f *Dict.C
	@rm -f *Dict.h
	@rm -f *.o
	@rm -f *.so
	@rm -f $(OCA_EXE)
	@echo "Done."

soc2oca: soc2oca.o $(OCAL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
	 $< ${DEPLIBS} ${RATLIB} ${OCALIB}

oca2fit: oca2fit.o $(OCAL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
	 $< ${DEPLIBS} ${RATLIB} ${OCALIB}

oca2marquardt: oca2marquardt.o $(OCAL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
	 $< ${DEPLIBS} ${RATLIB} ${OCALIB}

oca2fit2eff: oca2fit2eff.o $(OCAL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
	 $< ${DEPLIBS} ${RATLIB} ${OCALIB}

makedatastore: makedatastore.o $(OCAL)
	@echo " == Executing $@ =="
	$(CXX) $(CXXFLAGS) -o $@ \
	 $< ${DEPLIBS} ${RATLIB} ${OCALIB}

$(OCAL): $(OCAO) $(DICT_O)
	@echo " == Executing $@ =="
	@echo "linking $(OCAL)"
	$(CXX) $(LDFLAGS) $(LDOPTS) \
	-o $@ ${DEPLIBS} ${RATLIB} $(OCAO) $(DICT_O) $(LINKCMD)

# not sure if we really need ${DEPLIBS} up above here, we're only linking all the OCA object files

$(DICT_C): $(DICT_H) $(OCAO) $(OCAH)
	@echo " == Executing $@ =="
	@rm -f $@
	@echo "Generating $@ dictionary"
	rootcint -f $@ -c -p $(RATCFLAGS) $(OCAH) $<
	@echo "Done with $@ ..."


%.o:	%.cc %.hh
#	@echo Special rule for .cc files with headers
	$(CXX) ${PICFLAG} ${DEPCFLAGS} ${RATCFLAGS} ${OCACFLAGS} -DCOMPILED_ROOT -c $<

%.o:	%.C %.h
#	@echo Special rule for .C files with headers
	$(CXX) ${PICFLAG} ${DEPCFLAGS} ${RATCFLAGS} ${OCACFLAGS} -DCOMPILED_ROOT -c $<

%.o:	%.C
#	@echo Rule for .C files
	$(CXX) ${PICFLAG} ${DEPCFLAGS} ${RATCFLAGS} ${OCACFLAGS} -DCOMPILED_ROOT -c $<

%.o:	%.cc
#	@echo Rule for .cc files
	$(CXX) ${PICFLAG} ${DEPCFLAGS} ${RATCFLAGS} ${OCACFLAGS} -DCOMPILED_ROOT -c $<
