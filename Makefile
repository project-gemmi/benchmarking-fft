
# requires github.com/google/benchmark
CC=gcc-8
CXX=g++-8
#CC=clang-8
#CXX=clang++-8
LIBBENCHMARK=/usr/local/lib/libbenchmark.a
FLAGS=-Wall -Wextra -pedantic -O3 #-ffast-math

all: 1d meow_fft.o

OBJ=pocketfft.o kissfft.o pffft.o libmuFFT.a libmuFFT-sse.a libmuFFT-sse3.a libmuFFT-avx.a

1d: 1d.cpp $(OBJ)
	$(CXX) $(FLAGS) $< $(OBJ) -o $@ -lfftw3f $(LIBBENCHMARK) -pthread

pocketfft.o: pocketfft/pocketfft.c pocketfft/pocketfft.h
	$(CC) $(FLAGS) -c $< -o $@

kissfft.o: kissfft/kiss_fft.c kissfft/kiss_fft.h
	$(CC) $(FLAGS) -c $< -o $@

pffft.o: pffft/pffft.c pffft/pffft.h
	$(CC) $(FLAGS) -DPFFFT_SIMD_DISABLE -c $< -o $@

kfrfft.o: kfr/dft/impl/fft-impl-f32.cpp
	$(CXX) $(FLAGS) -c $< -o $@

meow_fft.o: meow.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f 1d *.o

.PHONY: all clean
