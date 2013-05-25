CC = g++ -g

.cxx.o:
	$(CC) -c $<

PROG = oopov
HEADER = global.h
OBJS = camera.o csg.o finish.o finite.o infinite.o lightsrc.o main.o \
       misc.o normal.o parser.o pigment.o texture.o voronoi.o
LEX = pov
YACC = pov

$(PROG) : $(OBJS)
	$(CC) -o $(PROG) -L/usr/local/lib $(OBJS) -ll -ly -lm -lg++

voronoi.o : voronoi.cxx voronoi.h $(HEADER)

$(OBJS) : $$(@:.o=.cxx) $(HEADER)

parser.cxx : y.tab.c lex.yy.c
	cat y.tab.c lex.yy.c > parser.cxx
	touch parser.cxx

y.tab.c: $(YACC).y $(HEADER)
	yacc $(YACC).y

lex.yy.c: $(LEX).l
	lex $(LEX).l
