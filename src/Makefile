#####################################################################
# The following lines should be set to suit the local config
#####################################################################

# target program 
PROG = move

# extra paths (include + libs)
EXTRA_INCS = -I/opt/local/include
EXTRA_LIBS = -L/opt/local/lib
KILOLIB_PATH = $(LOCALDIR)/lib

#####################################################################
# The following lines may be modified but probably not 
#####################################################################

# some aliases commands 
PKGCONFIG = pkg-config
RM = rm -f

# compiler for simulator
SIM_CC = gcc

# binutils for avr targets
AVRCC = avr-gcc
AVRAR = avr-ar
AVROC = avr-objcopy
AVROD = avr-objdump
AVRUP = avrdude

#####################################################################
# Code bellow should not be modified
#####################################################################

# list of c files in this project
SOURCES = $(PROG).c 

# The name of the executable file
SIM_EXE= $(PROG)
ROB_EXE= $(PROG).hex

# targets to make 
all: $(ROB_EXE) $(SIM_EXE) 

clean :
	$(RM) *.o *.elf

cleanall: clean
	$(RM) $(SIM_EXE) $(ROB_EXE)

# Makefile syntax (reminder)
# $@ left-hand side of :
# $^ right-hand side of :
# $< first item of right-hand side of :

#####################################################################
# Compiling code for simulation
#####################################################################

# kilombo paths 
SIMHEADERS = $(shell $(PKGCONFIG) --define-prefix --cflags kilombo)
SIMLIBS = $(shell $(PKGCONFIG) --define-prefix --libs kilombo)

# sdl paths 
SDLHEADER = $(shell sdl-config --cflags)
SDLLIBS = $(shell sdl-config --libs)

# compilation and linking flags 
SIM_CFLAGS = -c -g -O2 -Wall -std=c99 $(SIMHEADERS) $(SDLHEADER) $(EXTRA_INCS)
SIM_LFLAGS = $(EXTRA_LIBS) $(SIMLIBS) $(SDLLIBS) -lm -ljansson



#automatically make a list of object files from the list of .c files
OBJECTS=$(SOURCES:.c=.o)

#general rule for compiling a .c file to .o
.c.o:
	@printf "Compiling for simulator ... "
	$(SIM_CC) $(SIM_CFLAGS) $< -o $@
	@echo "done"

$(SIM_EXE): $(OBJECTS)
	@printf "Linking for simulator ... "
	$(SIM_CC)  $(SIM_LFLAGS) -o $@  $(OBJECTS) 
	@echo "done"

#####################################################################
# Compilation for real robots
#####################################################################

# Paths to the official kilolib, used for real bots only
KILOHEADERS=$(shell $(PKGCONFIG) --define-prefix --cflags kilolib)
KILOLIB = $(KILOLIB_PATH)/kilolib.a

#PFLAGS = -P usb -c avrispmkII # user to reprogram OHC
REALCFLAGS = -mmcu=atmega328p -Wall -gdwarf-2 -O3
REALCFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
REALCFLAGS += -DF_CPU=8000000 $(KILOHEADERS) $(SIMHEADERS)  -DKILOBOT
REALCFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm   #For a floating point printf

REALFLASH = -R .eeprom -R .fuse -R .lock -R .signature
REALEEPROM = -j .eeprom --set-section-flags=.eeprom="alloc,load" 
REALEEPROM += --change-section-lma .eeprom=0

%.lss: %.elf
	$(AVROD) -d -S $< > $@

%.hex: %.elf
	@printf "Making hex image ... "	
	@$(AVROC) -O ihex $(REALFLASH) $< $@
	@echo  "done"

%.eep: %.elf
	$(AVROC) -O ihex $(REALEEPROM) $< $@

%.bin: %.elf
	$(AVROC) -O binary $(REALFLASH) $< $@ 

$(PROG).elf: $(SOURCES) $(KILOLIB)
	@printf "Compiling for robots ... "
	@$(AVRCC)  $(REALCFLAGS)  -o $@ $^ 
	@echo  "done"

