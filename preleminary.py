from timeit import repeat
import numpy
import scipy.fftpack
import pyfftw

def create_array(*size):
    #return numpy.random.rand(120, 128, 2*96).view(dtype=complex)
    arr = pyfftw.empty_aligned(size, dtype='complex64')
    arr[:] = numpy.random.randn(*size) + 1j*numpy.random.randn(*size)
    return arr

small = create_array(120, 128, 96)
big = create_array(416, 416, 256)

pyfftw.interfaces.cache.enable()

def numpy_fft(a):
    b = numpy.fft.fftn(a)
    c = numpy.fft.ifftn(b)
    #print(numpy.max(c - a))

def scipy_fft(a):
    b = scipy.fftpack.fftn(a)
    c = scipy.fftpack.ifftn(b)
    #print(numpy.max(c - a))

def fftw_fft(a):
    b = pyfftw.interfaces.numpy_fft.fftn(a)
    c = pyfftw.interfaces.numpy_fft.ifftn(b)

def run(fft):
    return (min(repeat(fft+'(small)', repeat=20, number=1, globals=globals())),
            min(repeat(fft+'(big)', repeat=3, number=1, globals=globals())))

print('lib   120x128x96 416x256x416')
print('mkl    %7.3f    %7.3f' % run('numpy_fft'))
print('scipy  %7.3f    %7.3f' % run('scipy_fft'))
print('pyfftw %7.3f    %7.3f' % run('fftw_fft'))
