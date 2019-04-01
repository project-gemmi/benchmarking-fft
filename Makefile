
# requires github.com/google/benchmark
CC=gcc-8
CXX=g++-8
#CC=clang-8
#CXX=clang++-8
LIBBENCHMARK=/usr/local/lib/libbenchmark.a
FLAGS=-Wall -Wextra -pedantic -O3 #-ffast-math

all: 1d 1d-r meow_fft.o

OBJ=pocketfft.o kissfft.o pffft.o libmuFFT.a libmuFFT-sse.a libmuFFT-sse3.a libmuFFT-avx.a

1d: 1d.cpp $(OBJ)
	$(CXX) $(FLAGS) $< $(OBJ) -o $@ -lfftw3f $(LIBBENCHMARK) -pthread

plan1d: plan1d.cpp $(OBJ)
	$(CXX) $(FLAGS) $< $(OBJ) -o $@ -lfftw3f $(LIBBENCHMARK) -pthread

1d-r: 1d-r.cpp $(OBJ) kissfftr.o
	$(CXX) $(FLAGS) $< kissfftr.o $(OBJ) -o $@ -lfftw3f $(LIBBENCHMARK) -pthread

pocketfft.o: pocketfft/pocketfft.c pocketfft/pocketfft.h
	$(CC) $(FLAGS) -c $< -o $@

kissfft.o: kissfft/kiss_fft.c kissfft/kiss_fft.h
	$(CC) $(FLAGS) -c $< -o $@
kissfftr.o: kissfft/tools/kiss_fftr.c
	$(CC) $(FLAGS) -Ikissfft -c $< -o $@

pffft.o: pffft/pffft.c pffft/pffft.h
	$(CC) $(FLAGS) -UPFFFT_SIMD_DISABLE -c $< -o $@

kfrfft.o: kfr/dft/impl/fft-impl-f32.cpp
	$(CXX) $(FLAGS) -c $< -o $@

meow.c:
	echo '#define MEOW_FFT_IMPLEMENTAION 1' > $@
	echo '#include "meow_fft/meow_fft.h"'  >> $@

meow_fft.o: meow.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f 1d 1d-r plan1d *.o

.PHONY: all clean
