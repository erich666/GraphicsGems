LIBFILE = ../gemslib.a

CFLAGS = $(GENCFLAGS) -I..

OFILES = clip.o bio.o cross.o

$(LIBFILE): $(OFILES)
	ar rcs $(LIBFILE) $(OFILES)

clean:
	/bin/rm -f clip.o bio.o cross.o

$(OFILES): line.h ../GraphicsGems.h
