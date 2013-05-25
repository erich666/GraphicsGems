# FILENAME:  Makefile  [revised 18 AUG 90]
# 
# AUTHOR:  Kelvin Thompson
# 
# DESCRIPTION:  Makefile for anti-aliased line rendering demo.

# locations of Utah RLE information
UTAH_RLE_INCLUDE_DIR = /usr/contrib/include
UTAH_RLE_LIB_FILE = /usr/contrib/lib/librle.a

CFLAGS = -I$(UTAH_RLE_INCLUDE_DIR)

OBJS = AAMain.o AALines.o AATables.o utah.o

.o :	.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $<

AALine : $(OBJS)
	cc $(CFLAGS) -o $@ $(OBJS) $(UTAH_RLE_LIB_FILE) -lm

clean:
	/bin/rm -f $(OBJS) AALine
