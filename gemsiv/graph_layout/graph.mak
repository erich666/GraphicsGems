.AUTODEPEND

.PATH.obj = .

#		*Translator Definitions*
CC = bcc -v -W -vi- -wpro -weas -wpre -n. -I$(INCLUDEPATH) -L$(LIBPATH)
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = C:\BORLANDC\LIB
INCLUDEPATH = C:\BORLANDC\INCLUDE


#		*Implicit Rules*
.cpp.obj:
  $(CC) -c -DMSWINDOWS {$< }

#		*List Macros*

OBJS = fileio.obj layout.obj graph.obj mswindow.obj vector.obj

#		*Explicit Rules*
graph: $(OBJS)
  $(TLINK) /v/x/c/P-/Twe/L$(LIBPATH) @&&|
c0ws.obj+
fileio.obj+
layout.obj+
graph.obj+
mswindow.obj+
vector.obj
graph
		# no map file
mathws.lib+
import.lib+
cws.lib

|
  RC  .\graph.exe


#		*Individual File Dependencies*
mswindow.obj: mswindow.cpp 

fileio.obj: fileio.cpp mswindow.hxx vector.hxx defines.h graph.hxx fileio.hxx

graph.obj: graph.cpp mswindow.hxx vector.hxx defines.h graph.hxx

layout.obj: layout.cpp mswindow.hxx vector.hxx defines.h graph.hxx 

vector.obj: vector.cpp vector.hxx 

