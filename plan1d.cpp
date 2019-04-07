// FFT plan generation microbenchmark

#include <complex>
#include <cassert>
#include <benchmark/benchmark.h>

#include <fftw3.h>

//#include "kfr/dft.hpp"
#include "kissfft/kiss_fft.h"
#include "pocketfft/pocketfft.h"
#include "pffft/pffft.h"
#include "muFFT/fft.h"
#define MEOW_FFT_IMPLEMENTATION 1
#include "meow_fft/meow_fft.h"

static const bool no_simd = false;

static size_t N;
static std::vector<std::complex<float>> input;

static void bm_fftw3(benchmark::State& state, int flag) {
  if (no_simd)
    flag |= FFTW_NO_SIMD;
  std::vector<std::complex<float>> vout(N);
  fftwf_complex* out = reinterpret_cast<fftwf_complex*>(&vout[0]);
  while (state.KeepRunning()) {
    fftwf_plan plan = fftwf_plan_dft_1d(
                            N, reinterpret_cast<fftwf_complex*>(input.data()),
                            out, FFTW_FORWARD, flag);
    benchmark::DoNotOptimize(plan);
    fftwf_destroy_plan(plan);
  }
}

static void bm_fftw3_meas(benchmark::State& state) {
  bm_fftw3(state, FFTW_MEASURE);
}

static void bm_fftw3_est(benchmark::State& state) {
  bm_fftw3(state, FFTW_ESTIMATE);
}

static void bm_kissfft(benchmark::State& state) {
  std::vector<std::complex<float>> vout(N);
  while (state.KeepRunning()) {
    kiss_fft_cfg cfg = kiss_fft_alloc(N, /*inverse*/false, 0, 0);
    benchmark::DoNotOptimize(cfg);
    kiss_fft_free(cfg);
  }
}

static void bm_pocketfft(benchmark::State& state) {
  while (state.KeepRunning()) {
    cfft_plan plan = make_cfft_plan(N);
    benchmark::DoNotOptimize(plan);
    destroy_cfft_plan(plan);
  }
}

static void bm_meowfft(benchmark::State& state) {
  while (state.KeepRunning()) {
    size_t workset_bytes = meow_fft_generate_workset(N, NULL);
    Meow_FFT_Workset* cfg = (Meow_FFT_Workset*) malloc(workset_bytes);
    meow_fft_generate_workset(N, cfg);
    benchmark::DoNotOptimize(cfg);
    free(cfg);
  }
}

// only for N=(2^a)*(3^b)*(5^c), a >= 5, b >=0, c >= 0
// input/output must be aligned to 16 bytes
static void bm_pffft(benchmark::State& state) {
  while (state.KeepRunning()) {
    PFFFT_Setup* cfg = pffft_new_setup(N, PFFFT_COMPLEX);
    if (cfg == 0)
      return;
    benchmark::DoNotOptimize(cfg);
    pffft_destroy_setup(cfg);
  }
}

/*
static void bm_kfr(benchmark::State& state) {
  std::vector<kfr::u8> temp(plan.temp_size);
  while (state.KeepRunning()) {
    {
      kfr::dft_plan<float> plan(N);
      benchmark::DoNotOptimize(plan);
    }
  }
}
*/

bool is_power_of_two(size_t x)
{
    return (x & (x - 1)) == 0;
}
static void bm_mufft(benchmark::State& state) {
  if (!is_power_of_two(N)) return;
  int flags = 0;
  if (no_simd)
    flags = MUFFT_FLAG_CPU_NO_SIMD;
  while (state.KeepRunning()) {
    mufft_plan_1d *plan = mufft_create_plan_1d_c2c(N, MUFFT_FORWARD, flags);
    benchmark::DoNotOptimize(plan);
    mufft_free_plan_1d(plan);
  }
}


int main(int argc, char** argv) {
  if (argc < 2) {
    printf("Call it with size as an argument.\n");
    return 1;
  }
  N = std::stoi(argv[argc-1]);
  input.resize(N);
  float c = 3.1;
  for (std::complex<float>& x : input) {
    c += 0.3;
    x = {c, 2 * c - c * c};
  }

  benchmark::RegisterBenchmark("fftw3 est.", bm_fftw3_est);
  benchmark::RegisterBenchmark("fftw3 meas.", bm_fftw3_meas);
  benchmark::RegisterBenchmark("mufft", bm_mufft);
  benchmark::RegisterBenchmark("pffft", bm_pffft);
  benchmark::RegisterBenchmark("pocketfft", bm_pocketfft);
  benchmark::RegisterBenchmark("meow_fft", bm_meowfft);
  benchmark::RegisterBenchmark("kissfft", bm_kissfft);
  //benchmark::RegisterBenchmark("kfr", bm_kfr);
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}

// vim:sw=2:ts=2:et
