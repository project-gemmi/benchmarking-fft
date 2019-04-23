// 1D R2C FFT microbenchmark

#include <complex>
#include <cassert>
#include <benchmark/benchmark.h>

#include <fftw3.h>

#include "kissfft/kiss_fft.h"
#include "kissfft/kiss_fftr.h"
#include "pocketfft/pocketfft.h"
#include "pffft/pffft.h"
#include "muFFT/fft.h"
#include "meow_fft/meow_fft.h"

static const bool no_simd = false;

static std::vector<float> input;
static std::vector<double> inputd;

static void bm_fftw3(benchmark::State& state, int flag) {
  if (no_simd)
    flag |= FFTW_NO_SIMD;
#if 0
  std::vector<std::complex<float>> vout(input.size());
  fftwf_complex* out = reinterpret_cast<fftwf_complex*>(&vout[0]);
  fftwf_plan plan = fftwf_plan_dft_r2c_1d(input.size(), input.data(),
                                          out, flag);
#else
  float* in2 = fftwf_alloc_real(input.size());
  memcpy(in2, input.data(), input.size() * sizeof(input[0]));
  fftwf_complex* out = fftwf_alloc_complex(input.size());
  fftwf_plan plan = fftwf_plan_dft_r2c_1d(input.size(), in2, out, flag);
#endif
  //fftwf_print_plan(plan);

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

static void bm_kissfft(benchmark::State& state) {
  std::vector<std::complex<float>> vout(input.size());
  kiss_fft_cpx* out = reinterpret_cast<kiss_fft_cpx*>(&vout[0]);
  kiss_fftr_cfg cfg = kiss_fftr_alloc(input.size(), /*inverse*/false, 0, 0);
  while (state.KeepRunning()) {
    kiss_fftr(cfg, input.data(), out);
    benchmark::DoNotOptimize(out);
  }
  kiss_fftr_free(cfg);
}

static void bm_pocketfft(benchmark::State& state) {
  std::vector<std::complex<double>> vout(inputd.size());
  double* out = reinterpret_cast<double*>(&vout[0]);
  pocketfft_plan_r plan = pocketfft_make_plan_r(inputd.size());
  while (state.KeepRunning()) {
    memcpy(out, inputd.data(), inputd.size() * sizeof(double));
    pocketfft_forward_r(plan, reinterpret_cast<double*>(&vout[0]), 1.);
    benchmark::DoNotOptimize(vout);
  }
  pocketfft_delete_plan_r(plan);
}

static void bm_meowfft(benchmark::State& state) {
  std::vector<std::complex<float>> vout(input.size());
  Meow_FFT_Complex* out = reinterpret_cast<Meow_FFT_Complex*>(&vout[0]);
  size_t workset_bytes = meow_fft_generate_workset_real(input.size(), NULL);
  Meow_FFT_Workset_Real* cfg = (Meow_FFT_Workset_Real*) malloc(workset_bytes);
  meow_fft_generate_workset_real(input.size(), cfg);
  while (state.KeepRunning()) {
    meow_fft_real(cfg, input.data(), out);
    benchmark::DoNotOptimize(out);
  }
  free(cfg);
}

// only for N=(2^a)*(3^b)*(5^c), a >= 5, b >=0, c >= 0
// input/output must be aligned to 16 bytes
static void bm_pffft(benchmark::State& state) {
  std::vector<std::complex<float>> vout(input.size());
  const float* in = reinterpret_cast<const float*>(&input[0]);
  float* out = reinterpret_cast<float*>(&vout[0]);
  assert(((size_t)in & 15) == 0);
  assert(((size_t)out & 15) == 0);
  PFFFT_Setup* cfg = pffft_new_setup(input.size(), PFFFT_REAL);
  float* work = (float*) pffft_aligned_malloc(input.size() * sizeof(float));
  if (cfg == 0)
    return;
  while (state.KeepRunning()) {
    pffft_transform_ordered(cfg, in, out, work, PFFFT_FORWARD);
    benchmark::DoNotOptimize(out);
  }
  pffft_aligned_free(work);
  pffft_destroy_setup(cfg);
}


bool is_power_of_two(size_t x)
{
    return (x & (x - 1)) == 0;
}
static void bm_mufft(benchmark::State& state) {
  if (!is_power_of_two(input.size())) return;
  const auto in = input.data();
  void* out = mufft_alloc(input.size() * sizeof(input[0]) + 1);
  assert(((size_t)in & 63) == 0);
  assert(((size_t)out & 63) == 0);
  int flags = 0;
  if (no_simd)
    flags = MUFFT_FLAG_CPU_NO_SIMD;
  mufft_plan_1d *plan = mufft_create_plan_1d_r2c(input.size(), flags);
  while (state.KeepRunning()) {
    mufft_execute_plan_1d(plan, out, in);
    benchmark::DoNotOptimize(out);
  }
  mufft_free_plan_1d(plan);
  mufft_free(out);
}


int main(int argc, char** argv) {
  if (argc < 2) {
    printf("Call it with size as an argument.\n");
    return 1;
  }
  int size = std::stoi(argv[argc-1]);
  input.resize(size);
  float c = 3.1;
  for (float& x : input) {
    c += 0.3;
    x = 2 * c - c * c;
  }
  inputd.resize(input.size());
  for (size_t i = 0; i != input.size(); ++i)
    inputd[i] = input[i];

  benchmark::RegisterBenchmark("fftw3 est.", bm_fftw3_est);
  benchmark::RegisterBenchmark("fftw3 meas.", bm_fftw3_meas);
  benchmark::RegisterBenchmark("mufft", bm_mufft);
  benchmark::RegisterBenchmark("pffft", bm_pffft);
  benchmark::RegisterBenchmark("pocketfft", bm_pocketfft);
  benchmark::RegisterBenchmark("meow_fft", bm_meowfft);
  benchmark::RegisterBenchmark("kissfft", bm_kissfft);
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}

// vim:sw=2:ts=2:et
