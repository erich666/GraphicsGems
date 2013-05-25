OBJS = NurbRefine.o NurbSubdiv.o NurbUtils.o NurbEval.o GGVecLib.o Main.o \
	FakeWindow.o

nurb_polyg: $(OBJS)
	$(CC) -o $@ $(OBJS) -lm

.c.o:
	$(CC) -c $<

NurbRefine.c: nurbs.h GraphicsGems.h
NurbSubdiv.c: nurbs.h drawing.h GraphicsGems.h
NurbUtils.c:  nurbs.h GraphicsGems.h
NurbEval.c:   nurbs.h GraphicsGems.h
GGVecLib.c:   GraphicsGems.h
Main.c:	      nurbs.h drawing.h GraphicsGems.h
