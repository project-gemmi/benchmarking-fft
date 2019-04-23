## Comparison of Fast Fourier Transform Libraries

(work in progress)

FFTW library has an [impressive list](http://www.fftw.org/benchfft/ffts.html)
of other FFT libraries that FFTW was benchmarked against.
Unfortunately, this list has not been updated since about 2005,
and the situation has changed.

FFTW is not the fastest one anymore, but it still has many advantages
and it is the reference point for other libraries.

[MKL](https://software.intel.com/en-us/mkl/features/fft)
(Intel Math Kernel Library) FFT is faster. It's not open-source, but it is freely redistributable.

[KFR](https://github.com/kfrlib/kfr) claims to be sometimes faster than MKL.

[FFTS](https://github.com/anthonix/ffts) (South) and
[FFTE](http://www.ffte.jp/) (East) are reported to be faster than FFTW,
at least in some cases.

[muFFT](https://github.com/Themaister/muFFT)
and [pffft](https://bitbucket.org/jpommier/pffft)
seem to have performance comparable to FFTW while being much simpler.

Libraries that are not SIMD-optimized, such as
[KissFFT](https://github.com/mborgerding/kissfft),
[meow_fft](https://github.com/JodiTheTigger/meow_fft)
and [pocketfft](https://gitlab.mpcdf.mpg.de/mtr/pocketfft)
tend to be slower, but are also worth considering.

I don't plan to use GPU for computations, so I won't cover here
[cuFFT](https://developer.nvidia.com/cufft),
[clFFT](https://github.com/clMathLibraries/clFFT),
[fbfft](https://github.com/facebook/fbcuda/tree/master/fbfft),
[GLFFT](https://github.com/Themaister/GLFFT), etc.

First, a quick look at these projects:

| Library | License | Since | Language | KLOC | Comments |
|---------|---------|-------|----------|------|----------|
|FFTW3    | GPL or $| 1997  |          | 100+ |          |
|MKL      | freeware| 20??  |          |   ?  |          |
|KFR      | GPL or $| 2016  |  C++14   | ~20  |          |
|FFTS     | MIT     | 2012  |  C       | 24   |          |
|FFTE     | custom  | 20??  |  Fortran |      |          |
|muFFT    | MIT     | 2015  |  C       | 2.5  |          |
|pffft    | BSD-like| 2011  |  C       | 1.5  |          |
|KissFFT  | 3-BSD   | 2003  |  C       | 0.7+1.1 | 1.1 for `tools/` |
|meow_fft | 0-BSD   | 2017  |  C       | 1.9  | single header |
|pocketfft| 3-BSD   | 2010? |  C       | 2.0  |          |

When I was looking for a fast
[JSON parser](https://github.com/project-gemmi/benchmarking-json/)
all the candidates were in C++. So I'm surprised to see only one
C++ project here.

### Selected features

I'm primarily after 3D complex-to-real and real-to-complex transforms.
For me, radices 2 and 3 are a must, 5 is useful, 7+ could also be useful.

r-N means radix-N (radix-4 and 8 are supported anyway as 2^N).  
"++" in the "prime" column means the Bluestein's algorithm.  
"+/-" for radix-7 means it's only for complex-to-complex transform.  
"s" and "d" denote single- and double-precision data.

| Library | r-3 | r-4 | r-5 | r-7 | r-8 | prime | 2D | 3D |  s  |  d  |
|---------|-----|-----|-----|-----|-----|-------|----|----|-----|-----|
|FFTW3    |  +  |  +  |  +  |  +  |  +  |  ++   | +  | +  |  +  |  +  |
|MKL      |  +  |  +  |  +  |  +  |  +  |  +?   | +  | +  |  +  |  +  |
|KFR      |  +  |  +  |  +  |  +  |  +  |   -   | -  | -  |  +  |  +  |
|FFTS     |  -  |  +  |  -  |  -  |  +  |  ++   | +  | +  |  +  |  +  |
|FFTE     |  +  |  +  |  +  |  -  |  +  |   -   | +  | +  |  -  |  +  |
|muFFT    |  -  |  +  |  -  |  -  |  +  |   -   | +  | -  |  +  |  -  |
|pffft    |  +  |  +  |  +  |  -  |  -  |   -   | -  | -  |  +  |  -  |
|KissFFT  |  +  |  +  |  +  |  -  |  -  |   +   | +  | +  |  +  |  +  |
|meow_fft |  +  |  +  |  +  |  -  |  +  |   +   | -  | -  |  +  |  -  |
|pocketfft|  +  |  +  |  +  | +/- |  -  |  ++   | -  | -  |  -  |  +  |

Additionally, for pffft compiled with enabled SIMD the fft size must be
a multiple of 16 for complex FFTs and 32 for real FFTs.

(let me know if I got anything wrong)

### Preleminary benchmark

I run all the benchmarks here on i7-5600U CPU.

Just to get an idea, I checked the speed of popular Python libraries
(the underlying FFT implementations are in C/C++/Fortran).
I used only two 3D array sizes, timing forward+inverse 3D
complex-to-complex FFT.
Here are results from the `preliminary.py` script on my laptop
(numpy and mkl are the same code before and after `pip install mkl-fft`):

    lib   120x128x96 416x256x416
    numpy    0.196      8.742
    mkl      0.009      0.504
    scipy    0.106      7.091
    pyfftw   0.060      4.442

Strange, the gap between MKL and FFTW should not be that big?

The 416x256x416 transform result from `3d.cpp` (see below) are:
0.65s for FFTW (measured), 1.4s for FFTW (estimated) and 4.3s for kissfft.

### Binary size

FFTW3 is a couple MB.  
PocketFFT and muFFT are about 80kB.
PocketFFT has more butterflies but muFFT has each in four versions (no-SIMD,
 SSE, SSE3 and AVX).  
pffft and meow_fft are about 32kB.
pffft has also four versions (no-SIMD, SSE1, AltiVec and NEON),
but only one is compiled.  
KissFFT (1D complex-to-complex) is only about 20kB.

### 1D performance

I'm benchmarking primarily lightweight libraries, and FFTW as the reference
point. All the benchmarks on this page are:

    Run on (4 X 3200 MHz CPU s)
    CPU Caches:
      L1 Data 32K (x2)
      L1 Instruction 32K (x2)
      L2 Unified 256K (x2)
      L3 Unified 4096K (x1)

**complex-to-complex** (from running `1d.cpp` compiled with GCC8 -O3)

                       n=256      n=384     n=480     n=512
    fftw3 est.         321 ns     499 ns   1538 ns    663 ns
    fftw3 meas.        274 ns     443 ns    883 ns    588 ns
    mufft              325 ns     n/a        n/a      719 ns
    pffft              585 ns    1014 ns   1329 ns   1255 ns
    fftw3 est. NS     1826 ns    3254 ns   4776 ns   4095 ns
    fftw3 meas. NS    1699 ns    2748 ns   3855 ns   3832 ns
    mufft NS          1784 ns     n/a        n/a     4024 ns
    pffft NS          1768 ns    2907 ns   4070 ns   3792 ns
    pocketfft         1690 ns    3035 ns   3633 ns   4009 ns
    meow_fft          2120 ns    4718 ns   5745 ns   4342 ns
    kissfft           2536 ns    4929 ns   6030 ns   6553 ns

NS = disabled SIMD

To a first approximation, SSE1 gives 3x speedup, AVX -- 6x.

Notes:

* I didn't compile FFTW3, I used binaries from Ubuntu 18.04
* `-ffast-math` doesn't seem to make a significant difference
* When using Clang 8 instead of GCC, PocketFFT is ~12% faster.
* All libraries are tested with single-precision numbers, except for
  PocketFFT which supports only double-precision.

I'm yet to check the accuracy of results.

**plan / setup** (`plan1d.cpp`)

Out of curiosity, I also checked how long it takes to generate a plan:

                      n=480       n=512
    fftw3 est.       17871 ns     9693 ns
    fftw3 meas.      31463 ns    25610 ns
    mufft              n/a       17103 ns
    pffft            12763 ns    13730 ns
    pocketfft         1267 ns     1274 ns
    meow_fft         15092 ns    13878 ns
    kissfft          15586 ns    15993 ns

PocketFFT has indeed very fast plan generation.

**real-to-complex** (`1d-r.cpp`)

                       n=480       n=512
    fftw3 est.         766 ns      814 ns
    fftw3 meas.        718 ns      681 ns
    mufft              n/a         511 ns
    pffft              634 ns      597 ns
    fftw3 est. NS     2442 ns     1921 ns
    fftw3 meas. NS    1812 ns     1735 ns
    mufft NS           n/a        2474 ns
    pffft NS          2025 ns     1963 ns
    pocketfft         2123 ns     2034 ns
    meow_fftt         3591 ns     2660 ns
    kissfft           3140 ns     2985 ns

NS = disabled SIMD

Notes:

* The output from different libraries is ordered differently.
* For small sizes (such as the ones above) R2C in FFTW (with SIMD)
  is slower than C2C. Strange, but I double checked the alignment of arrays
  and the muFFT benchmark shows the same anomaly on my computer.


### 2D performance

**complex-to-complex** (`2d.cpp`)

                   256x256    480x480
    fftw3 est.     1197 us    3002 us
    fftw3 meas.     306 us    1497 us
    mufft           259 us      n/a
    fftw3 est. NS  1559 us    5582 us
    fftw3 meas. NS 1033 us    4536 us
    mufft NS       1092 us      n/a
    pocketfft      1285 us    4311 us
    kissfft        1583 us    6903 us


### 3D performance

**complex-to-complex** (`3d.cpp`)

                   128x128x320  256x256x256   416x256x416
    fftw3 est.        41 ms       1171 ms       3152 ms
    fftw3 meas.       39 ms        194 ms        793 ms
    fftw3 est. NS    253 ms        987 ms       2720 ms
    fftw3 meas. NS   125 ms        443 ms       1476 ms
    kissfft          313 ms       1221 ms       5756 ms

**complex-to-complex** (`3d-r.cpp`)

                 128x128x320   256x256x256
    fftw3 est.       28 ms        219 ms
    fftw3 meas.      27 ms         98 ms
    fftw3 est. NS    88 ms        285 ms
    fftw3 meas. NS   62 ms        206 ms
    kissfft         121 ms        475 ms

**matrix transpose** (`transpose.cpp`)

NxN 2D FFT can be composed of 2N 1D FFTs of length N
and one transposition. NxNxN 3D FFT is made of 3N^2 1D FFTs
and two transpositions. So I was wondering how long it takes
to transpose a 3D matrix. I check here matrix of `complex<float>`.

                256x256x256
    assign          22 ms
    naive zyx      204 ms
    naive xzy       89 ms
    naive yxz       25 ms
    naive zxy       90 ms
    naive yzx      202 ms
    tiled zxy       49 ms
    in-place zxy    91 ms


Only the last transpose is in-place (and it is also tiled).  
Are two XYZ -> ZXY transpositions all that is needed?  
As with FFT, SIMD instructions could make it faster, but how much?

### WebAssembly

TODO
