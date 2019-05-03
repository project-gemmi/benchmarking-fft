// 3D C2C FFT microbenchmark

#include <complex>
#include <cassert>
#include <benchmark/benchmark.h>
#include <pocketfft/pocketfft.h>

#include <fftw3.h>
#include "kissfft/kiss_fftnd.h"

static const bool no_simd = false;

static int x;
static int y;
static int z;
static std::vector<std::complex<float>> input;

static void bm_fftw3(benchmark::State& state, int flag) {
  if (no_simd)
    flag |= FFTW_NO_SIMD;
  std::vector<std::complex<float>> vout(x * y * z);
  fftwf_complex* out = reinterpret_cast<fftwf_complex*>(&vout[0]);
  fftwf_complex* in = reinterpret_cast<fftwf_complex*>(&input[0]);
  //in = fftwf_alloc_complex(x * y * z);
  //out = fftwf_alloc_complex(x * y * z);
  fftwf_plan plan = fftwf_plan_dft_3d(x, y, z, in, out, FFTW_FORWARD, flag);
  while (state.KeepRunning()) {
    fftwf_execute(plan);
    benchmark::DoNotOptimize(out);
  }
  fftwf_destroy_plan(plan);
}

static void bm_fftw3_meas(benchmark::State& state) {
  bm_fftw3(state, FFTW_MEASURE);
}

static void bm_fftw3_est(benchmark::State& state) {
  bm_fftw3(state, FFTW_ESTIMATE);
}

static void bm_pocketfft(benchmark::State& state) {
  std::vector<std::complex<float>> vout(x * y * z);
  size_t n[]={(size_t)x, (size_t)y, (size_t)z};
  ptrdiff_t s = sizeof(input[0]);
  ptrdiff_t stride[]={y*z*s, z*s, s};
  size_t axes[]={0,1,2};
  while (state.KeepRunning()) {
    pocketfft_complex(3, n, stride, stride, 3, axes, 1,
                      input.data(), vout.data(), 1., 0);
    benchmark::DoNotOptimize(vout);
  }
}

static void bm_kissfft(benchmark::State& state) {
  std::vector<std::complex<float>> vout(x * y * z);
  kiss_fft_cpx* out = reinterpret_cast<kiss_fft_cpx*>(&vout[0]);
  const int dims[3] = {x, y, z};
  kiss_fftnd_cfg cfg = kiss_fftnd_alloc(dims, 3, /*inverse*/false, 0, 0);
  while (state.KeepRunning()) {
    kiss_fftnd(cfg, reinterpret_cast<const kiss_fft_cpx*>(&input[0]), out);
    benchmark::DoNotOptimize(out);
  }
  kiss_fft_free(cfg);
}


int main(int argc, char** argv) {
  if (argc < 4) {
    printf("Call it with 3D size as arguments.\n");
    return 1;
  }
  x = std::stoi(argv[argc-3]);
  y = std::stoi(argv[argc-2]);
  z = std::stoi(argv[argc-1]);
  input.resize(x * y * z);
  for (int i = 0; i < x * y; ++i)
    input[i] = {0.3f * i, 10.f * i - sqrtf(i)};

  benchmark::RegisterBenchmark("fftw3 est.", bm_fftw3_est);
  benchmark::RegisterBenchmark("fftw3 meas.", bm_fftw3_meas);
  benchmark::RegisterBenchmark("pocketfft", bm_pocketfft);
  benchmark::RegisterBenchmark("kissfft", bm_kissfft);
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}

// vim:sw=2:ts=2:et
