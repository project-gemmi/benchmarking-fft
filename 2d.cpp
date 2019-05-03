// 2D C2C FFT microbenchmark

#include <complex>
#include <cassert>
#include <benchmark/benchmark.h>

#include <fftw3.h>
#include "kissfft/kiss_fftnd.h"
#include "pocketfft/pocketfft.h"
#include "muFFT/fft.h"

static const bool no_simd = false;

static int x;
static int y;
std::complex<float>* input;

static void bm_fftw3(benchmark::State& state, int flag) {
  if (no_simd)
    flag |= FFTW_NO_SIMD;
  std::vector<std::complex<float>> vout(x * y);
  fftwf_complex* out = reinterpret_cast<fftwf_complex*>(&vout[0]);
  fftwf_plan plan = fftwf_plan_dft_2d(x, y,
                                      reinterpret_cast<fftwf_complex*>(input),
                                      out, FFTW_FORWARD, flag);
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
  std::vector<std::complex<float>> vout(x * y);
  size_t n[]={(size_t)x, (size_t)y};
  ptrdiff_t s = sizeof(input[0]);
  ptrdiff_t stride[] = {y*s, 1*s};
  size_t axes[] = {0, 1};
  while (state.KeepRunning()) {
    pocketfft_complex(2, n, stride, stride, 2, axes, 1,
                      input, vout.data(), 1., 0);
    benchmark::DoNotOptimize(vout);
  }
}

static void bm_kissfft(benchmark::State& state) {
  std::vector<std::complex<float>> vout(x * y);
  kiss_fft_cpx* out = reinterpret_cast<kiss_fft_cpx*>(&vout[0]);
  const int dims[2] = {x, y};
  kiss_fftnd_cfg cfg = kiss_fftnd_alloc(dims, 2, /*inverse*/false, 0, 0);
  while (state.KeepRunning()) {
    kiss_fftnd(cfg, reinterpret_cast<const kiss_fft_cpx*>(input), out);
    benchmark::DoNotOptimize(out);
  }
  kiss_fft_free(cfg);
}

bool is_power_of_two(size_t x)
{
    return (x & (x - 1)) == 0;
}
static void bm_mufft(benchmark::State& state) {
  if (!is_power_of_two(x) || !is_power_of_two(y)) return;
  void* out = mufft_alloc(x * y * sizeof(input[0]));
  assert(((size_t)input & 63) == 0);
  assert(((size_t)out & 63) == 0);
  int flags = 0;
  if (no_simd)
    flags = MUFFT_FLAG_CPU_NO_SIMD;
  mufft_plan_2d *plan = mufft_create_plan_2d_c2c(x, y, MUFFT_FORWARD, flags);
  while (state.KeepRunning()) {
    mufft_execute_plan_2d(plan, out, input);
    benchmark::DoNotOptimize(out);
  }
  mufft_free_plan_2d(plan);
  mufft_free(out);
}


int main(int argc, char** argv) {
  if (argc < 3) {
    printf("Call it with 2D size as arguments.\n");
    return 1;
  }
  x = std::stoi(argv[argc-2]);
  y = std::stoi(argv[argc-1]);
  input = (std::complex<float>*) mufft_alloc(x * y * 2 * sizeof(float));
  for (int i = 0; i < x * y; ++i)
    input[i] = {0.3f * i, 10.f * i - sqrtf(i)};

  benchmark::RegisterBenchmark("fftw3 est.", bm_fftw3_est);
  benchmark::RegisterBenchmark("fftw3 meas.", bm_fftw3_meas);
  benchmark::RegisterBenchmark("mufft", bm_mufft);
  benchmark::RegisterBenchmark("pocketfft", bm_pocketfft);
  benchmark::RegisterBenchmark("kissfft", bm_kissfft);
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  mufft_free(input);
}

// vim:sw=2:ts=2:et
