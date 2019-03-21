I'm yet to do a proper benchmark.
For now these are just notes for myself.

FFTW library has an [impressive list](http://www.fftw.org/benchfft/ffts.html)
of other FFT libraries that FFTW was benchmarked against.
Unfortunately, this list has not been updated since about 2005,
and the situation today (2019) is different.

Nowadays, FFTW is not the fastest one, but it is the reference point for other libraries.

[MKL](https://software.intel.com/en-us/mkl/features/fft)
(Intel Math Kernel Library) FFT is faster. It's not open-source, but it is freely redistributable.

[KFR](https://github.com/kfrlib/kfr) claims to be even faster than MKL.

[FFTS](https://github.com/anthonix/ffts) may be slightly faster than FFTW.

[muFFT](https://github.com/Themaister/muFFT)
and [pffft](https://bitbucket.org/jpommier/pffft)
seem to have performance similar to FFTW while being much simpler.

Libraries that are not SIMD-optimized, such as
[KissFFT](https://github.com/mborgerding/kissfft),
[meow_fft](https://github.com/JodiTheTigger/meow_fft)
and [pocketfft](https://gitlab.mpcdf.mpg.de/mtr/pocketfft)
tend to be slower, but are also worth considering.

I don't plan using GPU for computations.

First, a quick look at these projects:

| Library | License | Since | Language | Comments |
|---------|---------|-------|----------|----------|
|FFTW     | GPL or $| 1997  |          |          |
|MKL      | freeware| 20??  |          |          |
|KFR      | GPL or $| 2016  |  C++14   | header-only |
|FFTS     | MIT     | 2012  |  C       |          |
|muFFT    | MIT     | 2015  |  C       |          |
|pffft    | BSD-like| 2011  |  C       |          |
|KissFFT  | 3-BSD   | 2003  |  C       |          |
|meow_fft | 0-BSD   | 2017  |  C       | single header |
|pocketfft| 3-BSD   | 2010? |  C       |          |

Features, source code size and binary size

TODO

Performance and accuracy

TODO
