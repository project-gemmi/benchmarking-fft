
# requires github.com/google/benchmark
CC=gcc-8
CXX=g++-8
#CC=clang-8
#CXX=clang++-8
LIBBENCHMARK=/usr/local/lib/libbenchmark.a -pthread
LIBFFTW=-lfftw3f
#LIBFFTW=/home/wojdyr/local/src/fftw-3.3.8/.libs/libfftw3f.a
FLAGS=-Wall -Wextra -pedantic -I. -O3 #-ffast-math

all: 1d 1d-r meow_fft.o plan1d 2d 3d 3d-r transpose

OBJ_2D=pocketfft.o kissfft.o libmuFFT.a libmuFFT-sse.a libmuFFT-sse3.a libmuFFT-avx.a
OBJ=pffft.o meow_fft.o $(OBJ_2D)

1d: 1d.cpp $(OBJ)
	$(CXX) $(FLAGS) $< $(OBJ) -o $@ $(LIBFFTW) $(LIBBENCHMARK)

plan1d: plan1d.cpp $(OBJ)
	$(CXX) $(FLAGS) $< $(OBJ) -o $@ $(LIBFFTW) $(LIBBENCHMARK)

1d-r: 1d-r.cpp $(OBJ) kissfftr.o
	$(CXX) $(FLAGS) $< kissfftr.o $(OBJ) -o $@ $(LIBFFTW) $(LIBBENCHMARK)

2d: 2d.cpp $(OBJ_2D) kissfftnd.o
	$(CXX) $(FLAGS) $< kissfftnd.o $(OBJ_2D) -o $@ $(LIBFFTW) $(LIBBENCHMARK)

3d: 3d.cpp pocketfft.o kissfft.o kissfftnd.o
	$(CXX) $(FLAGS) $< pocketfft.o kissfftnd.o kissfft.o -o $@ $(LIBFFTW) $(LIBBENCHMARK)

3d-r: 3d-r.cpp pocketfft.o kissfft.o kissfftndr.o kissfftnd.o kissfftr.o
	$(CXX) $(FLAGS) $< pocketfft.o kissfftndr.o kissfftnd.o kissfftr.o kissfft.o -o $@ $(LIBFFTW) $(LIBBENCHMARK)

transpose: transpose.cpp
	$(CXX) $(FLAGS) $< -o $@ $(LIBBENCHMARK)

pocketfft.o: pocketfft/pocketfft.cc pocketfft/pocketfft.h
	$(CXX) $(FLAGS) -c $< -o $@

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
