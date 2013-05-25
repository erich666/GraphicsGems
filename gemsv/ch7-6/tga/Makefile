# An ANSI C compiler: gnu's "gcc"
CC = gcc

# The UNIX flag must be defined for UNIX implementations
#CPPFLAGS = -DUNIX -DX11 -I.

# Very important!  The first flag (struct-return) makes it safe to link
#  gcc objects with cc objects.
GCCFLAGS = -fpcc-struct-return 

# Object files wiil go in this subdirectory
OBJ_DIR	     =  .

# User-defined; to pass options in to the compilation, say things
#   like 'make CFLAGS=-g'.  Put flags here to make them permanent.

CFLAGS 	= -O2

#####################################################################

TARGET = libtga.a

#HFILES = alias.h lug.h lugconf.h lugfnts.h rla.h targa.h tga.h

OBJECTS =       \
bitmap.o    \
tga.o    \
in_out.o    \
memory.o    \
error.o    \
general.o    \
cnv.o \
hsl.o \
x11.o \
dither.o \
gif.o \
encodgif.o \
tobw.o \
#####################################################################


PATH_OBJECTS =  $(OBJECTS:%=$(OBJ_DIR)/%)

$(OBJ_DIR)/%.o: %.c
	$(CC) $(GCCFLAGS) $(CFLAGS) -c -o $(OBJ_DIR)/$*.o $*.c

$(TARGET):  $(PATH_OBJECTS)
	echo $(PATH_OBJECTS)
	ar rv $(TARGET) $(PATH_OBJECTS)
	ranlib $(TARGET)

remove:
	/bin/rm -f $(PATH_OBJECTS)
	/bin/rm -f *~

clean:  remove $(TARGET)

#$(PATH_OBJECTS): $(HFILES)
