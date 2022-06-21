## Comparison of Fast Fourier Transform Libraries

FFTW library has an [impressive list](http://www.fftw.org/benchfft/ffts.html)
of other FFT libraries that FFTW was benchmarked against.
Unfortunately, this list has not been updated since about 2005,
and the situation has changed.
(Update: Steven Johnson
[showed a new benchmark](https://youtu.be/mSgXWpvQEHE?t=2588)
during JuliaCon 2019. In his hands FFTW runs slightly faster
than Intel MKL. In my hands MKL is ~50% faster.
Maybe I didn't squeeze all the performance from FFTW.)

FFTW is not the fastest one anymore, but it still has many advantages
and it is the reference point for other libraries.

[MKL](https://software.intel.com/en-us/mkl/features/fft)
(Intel Math Kernel Library) FFT is significantly faster.
It's not open-source, but it is freely redistributable.
MKL has fantastic compatibility with FFTW
(no need to change the code, you just link it with MKL instead of fftw3)
and with NumPy (no need to change the code, just do `pip install mkl-fft`).

[KFR](https://github.com/kfrlib/kfr) also claims to be faster than FFTW,
but I read that in the latest version (3.0) it requires Clang for
top performance, so I didn't benchmark it.

[FFTS](https://github.com/anthonix/ffts) (South) and
[FFTE](http://www.ffte.jp/) (East) are reported to be faster than FFTW,
at least in some cases. I'd benchmark them if I had more time.

[muFFT](https://github.com/Themaister/muFFT)
and [pffft](https://bitbucket.org/jpommier/pffft)
seem to have performance comparable to FFTW while being much simpler.  
Update: there is also [PGFFT](https://www.shoup.net/PGFFT/) in this category.

Libraries that are not vectorized such as
[KissFFT](https://github.com/mborgerding/kissfft),
[meow_fft](https://github.com/JodiTheTigger/meow_fft)
tend to be slower, but are also worth considering.

[PocketFFT](https://gitlab.mpcdf.mpg.de/mtr/pocketfft)
is vectorized only for multi-dimensional transforms (or for doing
multiple 1D transforms). Unlike in other projects, it
uses `` __attribute__((vector_size(N))`` instead of intrinsics.
Which makes it platform independent, but requires GCC or Clang
for vectorization.

I don't plan to use GPU for computations, so I won't try
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
|PGFFT    | 2-BSD   | 2019  |  C++11   | 1.0  |          |
|KissFFT  | 3-BSD   | 2003  |  C       | 0.7+1.1 | 1.1 for `tools/` |
|meow_fft | 0-BSD   | 2017  |  C       | 1.9  | single header |
|pocketfft| 3-BSD   | 2010? |  C++     | 2.2  | single header |

Note: pocketfft was originally in C, but now the repository has a `cpp`
branch and I'm migrating my benchmarks to it.

When I was looking for a fast
[JSON parser](https://github.com/project-gemmi/benchmarking-json/)
all the candidates were in C++, but here it's mostly C.

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
|PGFFT    |  -  |  -  |  -  |  -  |  -  |  ++   | -  | -  |  -  |  +  |
|KissFFT  |  +  |  +  |  +  |  -  |  -  |   +   | +  | +  |  +  |  +  |
|meow_fft |  +  |  +  |  +  |  -  |  +  |   +   | -  | -  |  +  |  -  |
|pocketfft|  +  |  +  |  +  | +/- |  -  |  ++   | +  | +  |  +  |  +  |

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

This is before NumPy switched to PocketFFT. NumPy will use internally
PocketFFT from version 1.17, which is not released yet when I'm writing it.

(Update: I'm not planning on updating the results, but it's worth noting
that SciPy also switched to PocketFFT in version 1.4.0.
And added module `scipy.fft` with different API than the old `scipy.fftpack`.
While NumPy is using PocketFFT in C, SciPy adopted newer version in templated C++.)

MKL is here as fast as in the native benchmark below (`3d.cpp`)
while other libraries are slower than the slowest FFT run from C++.

### Binary size

FFTW3 is a couple MB.  
PocketFFT (C version) and muFFT are about 80kB.
PocketFFT has more butterflies but muFFT has each in four versions (no-SIMD,
 SSE, SSE3 and AVX).  
pffft and meow_fft are about 32kB.
pffft has also four versions (no-SIMD, SSE1, AltiVec and NEON),
but only one is compiled.  
KissFFT (1D complex-to-complex) is only about 20kB. PGFFT – a few kB more.

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

I tested libraries with disabled SIMD (vectorization)
because I plan to use FFT in WebAssembly which does not support
SIMD instructions yet.

To a first approximation, SSE1 gives 3x speedup, AVX -- 6x.

Notes:

* I didn't compile FFTW3, I used binaries from Ubuntu 18.04
* `-ffast-math` doesn't seem to make a significant difference
* When using Clang 8 instead of GCC, PocketFFT is ~12% faster.
* All libraries are tested with single-precision numbers, except for
  PocketFFT (C version) which supports only double-precision.

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
  can be slower than C2C.


### 2D performance

**complex-to-complex** (`2d.cpp`)

                   256x256    480x480
    fftw3 est.     1197 us    3002 us
    fftw3 meas.     306 us    1497 us
    mufft           259 us      n/a
    pocketfft       543 us    2270 us
    fftw3 est. NS  1559 us    5582 us
    fftw3 meas. NS 1033 us    4536 us
    mufft NS       1092 us      n/a
    kissfft        1583 us    5766 us

Here, PocketFFT is compiled with SSE1 support only.
It is faster when compiled with AVX support. I haven't tried AVX-512.

### 3D performance

Here I also tried Intel MKL 2019 through its FFTW interface.
No changes in the source code, only the linking command needs to be modified.

**complex-to-complex** (`3d.cpp`)

                   128x128x320  256x256x256   416x256x416
    MKL               38 ms        155 ms        492 ms
    fftw3 est.        41 ms        730 ms       1860 ms
    fftw3 meas.       39 ms        162 ms        727 ms
    pocketfft         79 ms        264 ms        939 ms
    fftw3 est. NS    253 ms        987 ms       2016 ms
    fftw3 meas. NS   125 ms        443 ms       1476 ms
    kissfft          216 ms        785 ms       4235 ms

**real-to-complex** (`3d-r.cpp`)

                 128x128x320   256x256x256    416x256x416  90x128x120
    MKL              17 ms         61 ms         185 ms        4 ms
    fftw3 est.       28 ms        219 ms         605 ms       10 ms
    fftw3 meas.      27 ms         98 ms         336 ms        7 ms
    pocketfft AVX    30 ms         97 ms         311 ms        8 ms
    pocketfft SSE    38 ms        126 ms         393 ms       10 ms
    fftw3 est. NS    88 ms        285 ms         770 ms       19 ms
    fftw3 meas. NS   62 ms        206 ms         715 ms       15 ms
    kissfft         112 ms        436 ms        2078 ms       27 ms

PocketFFT compiled with AVX support is as fast as FFTW3.

**matrix transpose** (`transpose.cpp`)

Out of curiosity, I've also checked how long it takes to transpose
a 3D matrix of type `complex<float>`.
Only the last transpose is in-place (and it is also tiled).

                256x256x256
    assign          22 ms
    naive zyx      204 ms
    naive xzy       89 ms
    naive yxz       25 ms
    naive zxy       90 ms
    naive yzx      202 ms
    tiled zxy       49 ms
    in-place zxy    91 ms

### Summary

For [my project](https://github.com/project-gemmi/gemmi/)
PocketFFT has the best trade-off between the size, features and performance.

I considered FFTW as a compile-time alternative,
but I'd need to change how my data is ordered.
The c2r transform in FFTW requires data
[contiguous in the halved direction](http://www.fftw.org/fftw3_doc/Real_002ddata-DFT-Array-Format.html). Simply transposing the data before FFT
would likely cancel out any performance benefit from using FFTW.

Update: I've been using PocketFFT for almost a year now.
I use the [cpp branch](https://gitlab.mpcdf.mpg.de/mtr/pocketfft/tree/cpp).
It's a perfect fit for my needs.
