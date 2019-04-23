
# requires github.com/google/benchmark
CC=gcc-8
CXX=g++-8
#CC=clang-8
#CXX=clang++-8
LIBBENCHMARK=/usr/local/lib/libbenchmark.a -pthread
FLAGS=-Wall -Wextra -pedantic -I. -O3 #-ffast-math

all: 1d 1d-r meow_fft.o plan1d 2d 3d 3d-r transpose

OBJ_2D=pocketfft.o kissfft.o libmuFFT.a libmuFFT-sse.a libmuFFT-sse3.a libmuFFT-avx.a
OBJ=pffft.o meow_fft.o $(OBJ_2D)

1d: 1d.cpp $(OBJ)
	$(CXX) $(FLAGS) $< $(OBJ) -o $@ -lfftw3f $(LIBBENCHMARK)

plan1d: plan1d.cpp $(OBJ)
	$(CXX) $(FLAGS) $< $(OBJ) -o $@ -lfftw3f $(LIBBENCHMARK)

1d-r: 1d-r.cpp $(OBJ) kissfftr.o
	$(CXX) $(FLAGS) $< kissfftr.o $(OBJ) -o $@ -lfftw3f $(LIBBENCHMARK)

2d: 2d.cpp $(OBJ_2D) kissfftnd.o
	$(CXX) $(FLAGS) $< kissfftnd.o $(OBJ_2D) -o $@ -lfftw3f $(LIBBENCHMARK)

3d: 3d.cpp kissfft.o kissfftnd.o
	$(CXX) $(FLAGS) $< kissfftnd.o kissfft.o -o $@ -lfftw3f $(LIBBENCHMARK)

3d-r: 3d-r.cpp kissfft.o kissfftndr.o kissfftnd.o kissfftr.o
	$(CXX) $(FLAGS) $< kissfftndr.o kissfftnd.o kissfftr.o kissfft.o -o $@ -lfftw3f $(LIBBENCHMARK)

transpose: transpose.cpp
	$(CXX) $(FLAGS) $< -o $@ $(LIBBENCHMARK)

pocketfft.o: pocketfft/pocketfft.c pocketfft/pocketfft.h
	$(CC) $(FLAGS) -c $< -o $@

kissfft.o: kissfft/kiss_fft.c kissfft/kiss_fft.h
	$(CC) $(FLAGS) -c $< -o $@
kissfftr.o: kissfft/tools/kiss_fftr.c
	$(CC) $(FLAGS) -Ikissfft -c $< -o $@
kissfftnd.o: kissfft/tools/kiss_fftnd.c
	$(CC) $(FLAGS) -Ikissfft -c $< -o $@
kissfftndr.o: kissfft/tools/kiss_fftndr.c
	$(CC) $(FLAGS) -Ikissfft -c $< -o $@

pffft.o: pffft/pffft.c pffft/pffft.h
	$(CC) $(FLAGS) -UPFFFT_SIMD_DISABLE -c $< -o $@

kfrfft.o: kfr/dft/impl/fft-impl-f32.cpp
	$(CXX) $(FLAGS) -c $< -o $@

meow.c:
	echo '#define MEOW_FFT_IMPLEMENTATION 1' > $@
	echo '#include "meow_fft/meow_fft.h"'  >> $@

meow_fft.o: meow.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f 1d 1d-r plan1d *.o

.PHONY: all clean
