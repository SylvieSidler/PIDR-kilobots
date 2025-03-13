# Make file for compiling a kilobot program, both for the
# kilobot simulator and for the real kilobot.
# The program can consist of multiple files. Files and paths to various libraries are
# configured below, these needs to be adapted to the system.
PROG=test

#list of c files in this project
SOURCES=$(PROG).cpp

#name of the executable file
EXECUTABLE=$(PROG)

# Includes kilosim + deps (libsfml, libomp, libhdf5)
DEPS_INC=$(shell pkgconf --define-prefix --cflags kilosim)
DEPS_INC+=$(shell pkgconf --cflags hdf5)

# libs 
DEPS_LIB=$(shell pkgconf --define-prefix --libs kilosim)
DEPS_LIB+=$(shell pkgconf --libs sfml-graphics) 
DEPS_LIB+=$(shell pkgconf --libs hdf5) # better to use /usr/bin/h5c++
DEPS_LIB+= -lhdf5_hl_cpp -lhdf5_cpp -lhdf5_hl 
DEPS_LIB+= -lgomp -lm 


CFLAGS = -c -O2 -g -DNDEBUG -march=native -Wall -Wextra -fopenmp 
CFLAGS += -I. $(DEPS_INC)

LFLAGS = -O2 -g -DNDEBUG

all: $(EXECUTABLE)


clean :
	rm -f *.o $(EXECUTABLE)

# # # # # # # # The following should be generic and not need changes

# make syntax:
# $@ left-hand side of :
# $^ right-hand side of :
# $< first item of right-hand side of :

#which c compiler to use. gcc works well.
CPP=g++

#automatically make a list of object files from the list of .c files
OBJECTS=$(SOURCES:.cpp=.o)


#general rule for compiling a .cpp file to .o
.cpp.o:
	$(CPP) $(CFLAGS) $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CPP) $(LFLAGS) $(OBJECTS) -o $@ $(DEPS_LIB) -lgomp -lm 


