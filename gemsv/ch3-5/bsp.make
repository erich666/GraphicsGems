# bsp.make
#
HEADERS	= bsp.h GraphicsGems.h
OBJS	= bspAlloc.o bspCollide.o bspPartition.o \
bspTree.o bspUtility.o mainBsp.o bspMemory.o

OPT	= -g 
LIBS	= -lm
BSP	= bsp
# ANSI-C: Use CC on Suns. Use cc -Aa on HPs.
CC	= CC

$(BSP)	: $(OBJS)
	$(CC) $(OPT) $(OBJS) $(LIBS) -o $(BSP)

bspAlloc.o	: $(HEADERS) bspAlloc.c
	$(CC) $(OPT) -c bspAlloc.c
bspCollide.o	: $(HEADERS) bspCollide.c
	$(CC) $(OPT) -c bspCollide.c
bspPartition.o	: $(HEADERS) bspPartition.c
	$(CC) $(OPT) -c bspPartition.c
bspTree.o	: $(HEADERS) bspTree.c
	$(CC) $(OPT) -c bspTree.c
bspUtility.o	: $(HEADERS) bspUtility.c
	$(CC) $(OPT) -c bspUtility.c
bspMemory.o	: $(HEADERS) bspMemory.c
	$(CC) $(OPT) -c bspMemory.c
mainBsp.o	: $(HEADERS) mainBsp.c
	$(CC) $(OPT) -c mainBsp.c
# bsp.make
