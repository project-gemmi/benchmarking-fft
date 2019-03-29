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

When I was looking for a fast
[JSON parser](https://github.com/project-gemmi/benchmarking-json/)
all the candidates were in C++. So I'm surprised to see only one
C++ project here. Its author wrote a post
[why he uses C++14](https://www.kfrlib.com/blog/how-c14-and-c17-help-to-write-faster-and-better-code-real-world-examples/).

#### Selected features.

I'm primarily after 3D complex-to-real and real-to-complex transforms.
For me, radices 2 and 3 are a must, 5 is useful, 7+ could also be useful.

r-N means radix-N (radix-4 and 8 are supported anyway as 2^N).  
"++" in the "prime" column means the Bluestein's algorithm.  
"s" and "d" denote single- and double-precision data.

| Library | r-3 | r-4 | r-5 | r-7 | r-8 | prime | 2D | 3D |  s  |  d  |
|---------|-----|-----|-----|-----|-----|-------|----|----|-----|-----|
|FFTW     |  +  |  +  |  +  |  +  |  +  |  ++   | +  | +  |  +  |  +  |
|MKL      |  +  |  +  |  +  |  +  |  +  |  +?   | +  | +  |  +  |  +  |
|KFR      |  +  |  +  |  +  |  +  |  +  |   -   | -  | -  |  +  |  +  |
|FFTS     |  -  |  +  |  -  |  -  |  +  |  ++   | +  | +  |  +  |  +  |
|FFTE     |  +  |  +  |  +  |  -  |  +  |   -   | +  | +  |  -  |  +  |
|muFFT    |  -  |  +  |  -  |  -  |  +  |   -   | +  | -  |  +  |  -  |
|pffft    |  +  |  +  |  +  |  -  |  -  |   -   | -  | -  |  +  |  -  |
|KissFFT  |  +  |  +  |  +  |  -  |  -  |   +   | +  | +  |  +  |  +  |
|meow_fft |  +  |  +  |  +  |  -  |  +  |   +   | -  | -  |  +  |  -  |
|pocketfft|  +  |  +  |  +  |  +  |  -  |  ++   | -  | -  |  -  |  +  |

(let me know if I got something wrong)

#### Preleminary benchmark

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

#### Performance, accuracy and binary size

TODO
