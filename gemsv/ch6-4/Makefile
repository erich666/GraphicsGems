# TEST FILE FOR vectorize, NOT FOR BOOK
#
# UNIX makefile for 'vectorize'
# AUTHOR: Jean-Francois DOUE
# LAST MODIFICATION: July 27, 1993
#

CC = gcc

C_FLAGS = -g -O

H_FILES = pt2.h chainCode.h
C_FILES = test.C chainCode.C vectorize.C pt2.C
O_FILES = test.o chainCode.o vectorize.o pt2.o

.SUFFIXES : .o .c .C

.C.o:
	$(CC) $(C_FLAGS) -c $@ $*.C

vectorize: $(O_FILES)
	$(CC) $(C_FLAGS) -o $@ $(O_FILES)
