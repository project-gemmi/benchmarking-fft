I'm yet to do a proper benchmark.
For now these are just notes for myself.

FFTW library has an [impressive list](http://www.fftw.org/benchfft/ffts.html)
of other FFT libraries that FFTW was benchmarked against.
Unfortunately, this list has not been updated since about 2005,
and the situation has changed.

FFTW is not the fastest one anymore, but it still has many advantages
and it is the reference point for other libraries.

[MKL](https://software.intel.com/en-us/mkl/features/fft)
(Intel Math Kernel Library) FFT is faster. It's not open-source, but it is freely redistributable.

[KFR](https://github.com/kfrlib/kfr) claims to be even faster than MKL.

[FFTS](https://github.com/anthonix/ffts) and
[FFTE](http://www.ffte.jp/) are reported to be faster than FFTW,
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
[GLFFT](https://github.com/Themaister/GLFFT), etc.

First, a quick look at these projects:

| Library | License | Since | Language | KLOC | Comments |
|---------|---------|-------|----------|------|----------|
|FFTW     | GPL or $| 1997  |          | 100+ |          |
|MKL      | freeware| 20??  |          |   ?  |          |
|KFR      | GPL or $| 2016  |  C++14   | ~20  | header-only |
|FFTS     | MIT     | 2012  |  C       | 24   |          |
|FFTE     | custom  | 20??  |  Fortran |      |          |
|muFFT    | MIT     | 2015  |  C       | 2.5  |          |
|pffft    | BSD-like| 2011  |  C       | 1.5  |          |
|KissFFT  | 3-BSD   | 2003  |  C       | 0.7+1.1 | 1.1 for `tools/` |
|meow_fft | 0-BSD   | 2017  |  C       | 1.9  | single header |
|pocketfft| 3-BSD   | 2010? |  C       | 2.0  |          |

Selected features. r-N means radix-N (radix-4 and 8 are supported anyway
as 2^N). "++" in the "prime" column means Bluestein's algorithm.


| Library | r-3 | r-4 | r-5 | r-7 | r-8 | prime | 2D | 3D |
|---------|-----|-----|-----|-----|-----|-------|----|----|
|FFTW     |  +  |  +  |  +  |  +  |  +  |  ++   | +  | +  |
|MKL      |  +  |  +  |  +  |  +  |  +  |  +?   | +  | +  |
|KFR      |  +  |  +  |  +  |  +  |  +  |   -   | -  | -  |
|FFTS     |  -  |  +  |  -  |  -  |  +  |  ++   | +  | +  |
|FFTE     |  +  |  +  |  +  |  -  |  +  |   -   | +  | +  |
|muFFT    |  -  |  +  |  -  |  -  |  +  |   -   | +  | -  |
|pffft    |  +  |  +  |  +  |  -  |  -  |   -   | -  | -  |
|KissFFT  |  +  |  +  |  +  |  -  |  -  |   +   | +  | +  |
|meow_fft |  +  |  +  |  +  |  -  |  +  |   +   | -  | -  |
|pocketfft|  +  |  +  |  +  |  +  |  -  |  ++   | -  | -  |

Performance, accuracy and binary size

TODO
