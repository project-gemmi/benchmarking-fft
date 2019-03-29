
# requires github.com/google/benchmark
CC=gcc-8
CXX=g++-8
LIBBENCHMARK=/usr/local/lib/libbenchmark.a
FLAGS=-Wall -Wextra -pedantic -O3 -Dkiss_fft_scalar=float

all: 1d

OBJ=pocketfft.o kissfft.o

1d: 1d.cpp $(OBJ)
	$(CXX) $(FLAGS) $< $(OBJ) -o $@ -lfftw3f $(LIBBENCHMARK) -pthread

pocketfft.o: pocketfft/pocketfft.c pocketfft/pocketfft.h
	$(CC) $(FLAGS) -c $< -o $@

kissfft.o: kissfft/kiss_fft.c kissfft/kiss_fft.h
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f 1d *.o

.PHONY: all clean
