# Module.mk for qoca module
#
# Author: P. Harvey 02/20/02

#ifneq ($(SNO_ROOT),)  # ignore this module if SNO_ROOT not defined

MODNAME      := QOCA
MODDIR       := qoca

QOCA_FLAGS   := $(CXXFLAGS) -DCOMPILED_ROOT

QOCA_DIR     := $(MODDIR)
QOCA_DIRS    := $(MODDIR)
QOCA_DIRI    := $(MODDIR)

QOCA_LH      := $(QOCA_DIRI)/$(MODNAME)_LinkDef.h
QOCA_DC      := $(QOCA_DIRS)/$(MODNAME)_Dict.C
QOCA_DO      := $(QOCA_DC:.C=.o)
QOCA_DH      := $(QOCA_DC:.C=.h)

QOCA_H       := $(filter-out $(QOCA_LH) $(QOCA_DH),$(wildcard $(QOCA_DIRI)/*.h))
QOCA_CXX     := $(wildcard $(QOCA_DIRS)/*.cxx)
QOCA_O       := $(QOCA_CXX:.cxx=.o)

QOCA_EXE     := bin/qoca_positionfit bin/reflecttime bin/qpath_runner bin/qocafit_runner bin/qocaextract_runner bin/qocaplots_runner bin/qocatitles_runner bin/qncdposfit

# remove necessary object files from build
IGNORE       := QMOCAFit doFit_chris QOCAFit_chris qpath_chris qoca_path avgeom

# add tdiff is snoman and snomanpp are built
ifneq ($(SNO_ROOT),)
QOCA_EXE     += bin/tdiff
else
IGNORE       += tdiff
endif

IGNORE_O     := $(patsubst %,$(QOCA_DIRS)/%.o,$(IGNORE))
IGNORE_H     := $(patsubst %,$(QOCA_DIRS)/%.h,$(IGNORE))
QOCA_O       := $(filter-out $(IGNORE_O),$(QOCA_O))
QOCA_H       := $(filter-out $(IGNORE_H),$(QOCA_H))

QOCA_QO      := $(filter $(QOCA_DIRS)/Q%,$(QOCA_O))

QOCA_DEP     := $(QOCA_O:.o=.d) $(QOCA_DO:.o=.d)

# used in the main Makefile
ALLHDRS      += $(patsubst $(QOCA_DIRI)/%.h,include/%.h,$(QOCA_H))
ALLLIBS      += $(QOCA_LIB)
ALLEXECS     += $(QOCA_EXE)

# include all dependency files
INCLUDEFILES += $(QOCA_DEP)

# include local MyConfig.mk file if required
-include $(QOCA_DIR)/MyConfig.mk

##### local rules #####

# we depend on all of our header files being up to date in the include directory
include/%.h:    $(QOCA_DIRI)/%.h
		$(COPY_HEADER) $< $@

# rule for compiling our source files
$(QOCA_DIRS)/%.o:    $(QOCA_DIRS)/%.cxx
	$(CXX) $(OPT) $(QOCA_FLAGS) $(ROOTINCS) -o $@ -c $<

# rule for building dependency files (necessary because of -DCOMPILED_ROOT)
$(QOCA_DIRS)/%.d: $(QOCA_DIRS)/%.cxx
	$(MAKEDEP) $@ "$(QOCA_FLAGS)" $< > $@

# rule for building qoca_path
bin/qoca_path: $(QOCA_DIRS)/qoca_path.o $(QOCA_DIRS)/lbmask.o $(QOCA_LIB) $(QTREE_LIB) $(QSNOED_LIB) $(QDISP_LIB)
		@echo "=== Linking $@ ==="
		$(F77LD) $(F77LDFLAGS) -o $@ $< $(QOCA_DIRS)/lbmask.o $(QSNOLIBDIRS) -lqoca -lqtree -lqsnoed\
			-lqdisp -lMinuit $(ROOTLIBS) $(XMLIBS) $(F77LIBS)

# rule for building tdiff
bin/tdiff: $(QOCA_DIRS)/tdiff.o $(QOCA_LIB) $(QC_LIB) $(QTREE_LIB) $(QSNOED_LIB) $(QDISP_LIB)
		@echo "=== Linking $@ ==="
		$(F77LD) $(F77LDFLAGS) -o $@ $< $(QSNOLIBDIRS) -lqoca -lqc -lsnomanpp -lqtree -lqsnoed -lqdisp -lMinuit\
			$(SNOMANLIBS) $(ROOTLIBS) $(CERNLIBS) $(XMLIBS) $(F77LIBS)

# rule for building other executables
bin/%: $(QOCA_DIRS)/%.o $(QOCA_LIB) $(QTREE_LIB) $(QSNOED_LIB) $(QDISP_LIB)
		@echo "=== Linking $@ ==="
		$(F77LD) $(F77LDFLAGS) -o $@ $< $(QSNOLIBDIRS) -lqoca -lqtree -lqsnoed -lqdisp -lMinuit\
			$(ROOTLIBS) $(XMLIBS) $(F77LIBS)

# rules for building dictionary
$(QOCA_DO):         $(QOCA_DC)
	$(CXX) $(NOOPT) $(QOCA_FLAGS) $(ROOTINCS) -I. -o $@ -c $<

$(QOCA_DC):         $(QOCA_H) $(QOCA_LH)
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ $(ROOTCINTFLAGS) $(QOCA_H) $(QOCA_LH)

# rule for building library
$(QOCA_LIB):        $(QOCA_QO) $(QOCA_DO) $(QOCA_LIBDEP)
	@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
	   "$(SOFLAGS)" libqoca.$(SOEXT) $@ "$(QOCA_QO) $(QOCA_DO)" \
	   "$(QOCA_LIBEXTRA)"

all-qoca:       $(QOCA_LIB)

clean-qoca:
		@rm -f $(QOCA_DIRS)/*~ $(QOCA_DIRS)/*.o
		@rm -f $(QOCA_DC) $(QOCA_DH) $(QOCA_DEP)

clean::         clean-qoca

#endif   # SNO_ROOT

#end

