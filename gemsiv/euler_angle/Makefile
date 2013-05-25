CC = gcc

EulerSample: EulerSample.o EulerAngles.o
	$(CC) -o EulerSample EulerSample.o EulerAngles.o -lm

clean:
	rm -f *.o EulerSample

EulerAngles.o: EulerAngles.h QuatTypes.h
EulerSample.o: EulerAngles.h QuatTypes.h
