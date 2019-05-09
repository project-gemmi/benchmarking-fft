
# requires github.com/google/benchmark
CC=gcc-8
CXX=g++-8
#CC=clang-8
#CXX=clang++-8

LIBBENCHMARK=/usr/local/lib/libbenchmark.a -pthread

LIBFFTW=-lfftw3f
#LIBFFTW=${HOME}/local/src/fftw-3.3.8/.libs/libfftw3f.a
#LIBFFTW=-Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_intel_ilp64.a ${MKLROOT}/lib/intel64/libmkl_sequential.a ${MKLROOT}/lib/intel64/libmkl_core.a -Wl,--end-group -lpthread -lm -ldl

ARCH=#-march=native
FLAGS=-Wall -Wextra -pedantic -I. -O3 $(ARCH)

LIBMU=libmuFFT.a libmuFFT-sse.a libmuFFT-sse3.a libmuFFT-avx.a
OBJ_3D=kissfftnd.o kissfft.o
OBJ_1D=$(LIBMU) kissfft.o pffft.o meow_fft.o
LIBS=$(LIBFFTW) $(LIBBENCHMARK)

all: 1d 1d-r meow_fft.o plan1d 2d 3d 3d-r transpose

1d: 1d.cpp $(OBJ_1D)
	$(CXX) $(FLAGS) $< $(OBJ_1D) -o $@ $(LIBS)

plan1d: plan1d.cpp $(OBJ_1D)
	$(CXX) $(FLAGS) $< $(OBJ_1D) -o $@ $(LIBS)

1d-r: 1d-r.cpp $(OBJ_1D) kissfftr.o
	$(CXX) $(FLAGS) $< kissfftr.o $(OBJ_1D) -o $@ $(LIBS)

2d: 2d.cpp $(LIBMU) $(OBJ_3D) 
	$(CXX) $(FLAGS) $< $(LIBMU) $(OBJ_3D) -o $@ $(LIBS)

3d: 3d.cpp $(OBJ_3D)
	$(CXX) $(FLAGS) $< $(OBJ_3D) -o $@ $(LIBS)

3d-r: 3d-r.cpp $(OBJ_3D) kissfftndr.o kissfftr.o
	$(CXX) $(FLAGS) $< kissfftndr.o kissfftr.o $(OBJ_3D) -o $@ $(LIBS)

transpose: transpose.cpp
	$(CXX) $(FLAGS) $< -o $@ $(LIBBENCHMARK)

pocketfft.o: pocketfft/pocketfft.cc pocketfft/pocketfft.h
	$(CXX) $(FLAGS) -march=native -c $< -o $@

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
