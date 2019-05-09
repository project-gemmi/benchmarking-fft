// 3D R2C FFT microbenchmark

#include <complex>
#include <cassert>
#include <benchmark/benchmark.h>
#include <pocketfft/pocketfft_hdronly.h>
#include <fftw3.h>
#include "kissfft/kiss_fftndr.h"

static const bool no_simd = false;

static int x;
static int y;
static int z;
static std::vector<float> input;

static void bm_fftw3(benchmark::State& state, int flag) {
  if (no_simd)
    flag |= FFTW_NO_SIMD;
  std::vector<std::complex<float>> vout(x * y * z);
  fftwf_complex* out = reinterpret_cast<fftwf_complex*>(&vout[0]);
  float* in = reinterpret_cast<float*>(&input[0]);
  //in = fftwf_alloc_real(x * y * z);
  //out = fftwf_alloc_complex(x * y * z);
  fftwf_plan plan = fftwf_plan_dft_r2c_3d(x, y, z, in, out, flag);
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
  int new_z = z / 2 + 1;
  std::vector<std::complex<float>> output(x * y * new_z);
  {
    pocketfft::shape_t shape{(size_t)x, (size_t)y, (size_t)z};
    pocketfft::shape_t shape2{(size_t)x, (size_t)y, (size_t)z/2+1};
    ptrdiff_t s = sizeof(float);
    pocketfft::stride_t stride{y * z * s, z * s, s};
    pocketfft::stride_t stride2{y * new_z * 2*s, new_z * 2*s, 2*s};
    while (state.KeepRunning()) {
      pocketfft::r2c<float>(shape, stride, stride2, 2, &input[0], &output[0], 1.);
      pocketfft::c2c<float>(shape2, stride2, stride2, {0,1},
                      true, &output[0], &output[0], 1.);
      benchmark::DoNotOptimize(output);
    }
  }
}

static void bm_kissfft(benchmark::State& state) {
  std::vector<std::complex<float>> vout(x * y * z);
  kiss_fft_cpx* out = reinterpret_cast<kiss_fft_cpx*>(&vout[0]);
  const int dims[3] = {x, y, z};
  kiss_fftndr_cfg cfg = kiss_fftndr_alloc(dims, 3, /*inverse*/false, 0, 0);
  while (state.KeepRunning()) {
    kiss_fftndr(cfg, reinterpret_cast<const float*>(&input[0]), out);
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
    input[i] = 10.f * i - sqrtf(i);

  benchmark::RegisterBenchmark("fftw3 est.", bm_fftw3_est);
  benchmark::RegisterBenchmark("fftw3 meas.", bm_fftw3_meas);
  benchmark::RegisterBenchmark("pocketfft", bm_pocketfft);
  benchmark::RegisterBenchmark("kissfft", bm_kissfft);
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}

// vim:sw=2:ts=2:et
