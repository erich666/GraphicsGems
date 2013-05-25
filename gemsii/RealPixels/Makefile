CFLAGS = -g
LIBS = -lm

ra_pr24:	color.o colrops.o header.o resolu.o rasterfile.h color.h
		cc $(CFLAGS) ra_pr24.c -o ra_pr24 \
			color.o colrops.o header.o resolu.o $(LIBS) 

color.o:	color.c color.h
		cc $(CFLAGS) -c color.c -o color.o

colrops.o:	colrops.c color.h
		cc $(CFLAGS) -c colrops.c -o colrops.o

header.o:	header.c
		cc $(CFLAGS) -c header.c -o header.o

resolu.o:	resolu.c color.h
		cc $(CFLAGS) -c resolu.c -o resolu.o

clean:
		/bin/rm -f color.o colrops.o header.o ra_pr24 resolu.o
