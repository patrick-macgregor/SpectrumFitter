# Makefile for spectrum_fitter

# Directories
PWD			:= $(shell pwd)
BIN_DIR     := ./bin
SRC_DIR     := ./src
LIB_DIR     := ./lib
INC_DIR     := ./include

# ROOT specific quantities N.B. uses version 6
ROOTVER   := $(shell root-config --version | head -c1)
ROOTDICT  := rootcling
DICTEXT   := _rdict.pcm

ROOTCPPFLAGS	:= $(shell root-config --cflags)
ROOTLDFLAGS		:= $(shell root-config --ldflags)
ROOTLIBS		:= $(shell root-config --glibs) -lRHTTP -lThread
LIBS			:= $(ROOTLIBS) $(LIBEXTRA)

# UNIX-specific flags
PLATFORM:=$(shell uname)
ifeq ($(PLATFORM),Darwin)
SHAREDSWITCH = -Qunused-arguments -shared -undefined dynamic_lookup -dynamiclib -Wl,-install_name,'@executable_path/../lib/'# NO ENDING SPACE
OSDEF = -DMACOSX
else
SHAREDSWITCH = -shared -Wl,-soname,# NO ENDING SPACE
OSDEF = -DLINUX
LIBEXTRA = -lrt
endif

# Flags for compiler.
CPPFLAGS	 = -c -Wall -Wextra $(ROOTCPPFLAGS) -g -fPIC
CPPFLAGS	+= -DUNIX -DPOSIX $(OSDEF)
INCLUDES	+= -I$(INC_DIR) -I.

# Linker.
LD          = $(shell root-config --ld)

# Flags for linker.
LDFLAGS 	+= $(ROOTLDFLAGS) -g

# Object files
OBJECTS = 	$(SRC_DIR)/CommandLineInterface.o \
			$(SRC_DIR)/Fit.o \
			$(SRC_DIR)/FitWriter.o \
			$(SRC_DIR)/InputFileProcessor.o \
			$(SRC_DIR)/MessageLogger.o \
			$(SRC_DIR)/Peak.o \
			$(SRC_DIR)/Spectrum.o \
			$(SRC_DIR)/SpectrumDrawer.o \
			$(SRC_DIR)/SpectrumFitter.o \
			$(SRC_DIR)/SpectrumIntegral.o

# Header files
DEPENDENCIES = 	$(INC_DIR)/CommandLineInterface.hh \
				$(INC_DIR)/Fit.hh \
				$(INC_DIR)/FitWriter.hh \
				$(INC_DIR)/InputFileProcessor.hh \
				$(INC_DIR)/MessageLogger.hh \
				$(INC_DIR)/Peak.hh \
				$(INC_DIR)/Spectrum.hh \
				$(INC_DIR)/SpectrumDrawer.hh \
				$(INC_DIR)/SpectrumFitter.hh \
				$(INC_DIR)/SpectrumIntegral.hh

# Recipes
all: $(BIN_DIR)/spectrum_fitter $(LIB_DIR)/libspectrum_fitter.so

$(LIB_DIR)/libspectrum_fitter.so: spectrum_fitter.o $(OBJECTS) spectrum_fitterDict.o
	mkdir -p $(LIB_DIR)
	$(LD) spectrum_fitter.o $(OBJECTS) spectrum_fitterDict.o $(SHAREDSWITCH)$@ $(LIBS) -o $@

$(BIN_DIR)/spectrum_fitter: spectrum_fitter.o $(OBJECTS) spectrum_fitterDict.o
	mkdir -p $(BIN_DIR)
	$(LD) -o $@ $^ $(LDFLAGS) $(LIBS)

spectrum_fitter.o: spectrum_fitter.cc
	$(CXX) $(CPPFLAGS) $(INCLUDES) $^

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cc $(INC_DIR)/%.hh
	$(CXX) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

spectrum_fitterDict.o: spectrum_fitterDict.cc spectrum_fitterDict$(DICTEXT) $(INC_DIR)/RootLinkDef.h
	mkdir -p $(BIN_DIR)
	mkdir -p $(LIB_DIR)
	$(CXX) -fPIC $(CPPFLAGS) $(INCLUDES) -c $<
	cp spectrum_fitterDict$(DICTEXT) $(BIN_DIR)/
	cp spectrum_fitterDict$(DICTEXT) $(LIB_DIR)/

spectrum_fitterDict.cc: $(DEPENDENCIES) $(INC_DIR)/RootLinkDef.h
	$(ROOTDICT) -f $@ -c $(INCLUDES) $(DEPENDENCIES) $(INC_DIR)/RootLinkDef.h

clean:
	rm -vf $(BIN_DIR)/spectrum_fitter $(SRC_DIR)/*.o $(SRC_DIR)/*~ $(INC_DIR)/*.gch *.o $(BIN_DIR)/*.pcm *.pcm $(BIN_DIR)/*Dict* *Dict* $(LIB_DIR)/*
