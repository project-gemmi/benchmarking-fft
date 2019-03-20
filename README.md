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

Libraries that are not SIMD-optimized,
such as KissFFT, meow_fft (single header!) and pocketfft
tend to be slower, but are also worth considering.

I don't plan using GPU for computations.
