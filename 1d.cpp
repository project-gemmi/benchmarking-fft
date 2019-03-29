// 1D FFT microbenchmark

#include <complex>
#include <benchmark/benchmark.h>

#include <fftw3.h>

#include "kissfft/kiss_fft.h"
#include "pocketfft/pocketfft.h"
#define MEOW_FFT_IMPLEMENTAION 1
#include "meow_fft/meow_fft.h"

static std::vector<std::complex<float>> input;
static std::vector<std::complex<double>> inputd;

static void bm_fftw3(benchmark::State& state) {
  std::vector<std::complex<float>> vout(input.size());
  fftwf_complex* out = reinterpret_cast<fftwf_complex*>(&vout[0]);
  fftwf_plan plan = fftwf_plan_dft_1d(
      input.size(), reinterpret_cast<fftwf_complex*>(input.data()),
      out, FFTW_FORWARD, FFTW_ESTIMATE/*FFTW_MEASURE*/);
  while (state.KeepRunning()) {
    fftwf_execute(plan);
    benchmark::DoNotOptimize(out);
  }
  fftwf_destroy_plan(plan);
}

static void bm_fftw3_incl_plan(benchmark::State& state) {
  std::vector<std::complex<float>> vout(input.size());
  fftwf_complex* out = reinterpret_cast<fftwf_complex*>(&vout[0]);
  while (state.KeepRunning()) {
    fftwf_plan plan = fftwf_plan_dft_1d(
        input.size(), reinterpret_cast<fftwf_complex*>(input.data()),
        out, FFTW_FORWARD, FFTW_ESTIMATE/*FFTW_MEASURE*/);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);
    benchmark::DoNotOptimize(out);
  }
}

static void bm_kissfft(benchmark::State& state) {
  std::vector<std::complex<float>> vout(input.size());
  kiss_fft_cpx* out = reinterpret_cast<kiss_fft_cpx*>(&vout[0]);
  kiss_fft_cfg cfg = kiss_fft_alloc(input.size(), /*inverse*/false, 0, 0);
  while (state.KeepRunning()) {
    kiss_fft(cfg, reinterpret_cast<const kiss_fft_cpx*>(input.data()), out);
    benchmark::DoNotOptimize(out);
  }
  kiss_fft_free(cfg);
}

static void bm_pocketfft(benchmark::State& state) {
  std::vector<std::complex<double>> vout(inputd.size());
  cfft_plan plan = make_cfft_plan(inputd.size());
  while (state.KeepRunning()) {
    vout = inputd;
    cfft_forward(plan, reinterpret_cast<double*>(&vout[0]), 1.);
    benchmark::DoNotOptimize(vout);
  }
  destroy_cfft_plan(plan);
}

static void bm_meowfft(benchmark::State& state) {
  std::vector<std::complex<float>> vout(input.size());
  Meow_FFT_Complex* out = reinterpret_cast<Meow_FFT_Complex*>(&vout[0]);
  size_t workset_bytes = meow_fft_generate_workset(input.size(), NULL);
  Meow_FFT_Workset* cfg = (Meow_FFT_Workset*) malloc(workset_bytes);
  meow_fft_generate_workset(input.size(), cfg);
  while (state.KeepRunning()) {
    meow_fft(cfg, reinterpret_cast<const Meow_FFT_Complex*>(input.data()), out);
    benchmark::DoNotOptimize(out);
  }
  free(cfg);
}

// TODO: muFFT w/ and w/o SIMD, pffft, KFR

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("Call it with size as an argument.\n");
    return 1;
  }
  int size = std::stoi(argv[argc-1]);
  input.resize(size);
  float c = 3.1;
  for (std::complex<float>& x : input) {
    c += 0.3;
    x = {c, 2 * c - c * c};
  }
  inputd.resize(input.size());
  for (size_t i = 0; i != input.size(); ++i)
    inputd[i] = input[i];

  benchmark::RegisterBenchmark("fftw3", bm_fftw3);
  benchmark::RegisterBenchmark("fftw3 incl. plan", bm_fftw3_incl_plan);
  benchmark::RegisterBenchmark("kissfft", bm_kissfft);
  benchmark::RegisterBenchmark("pocketfft", bm_pocketfft);
  benchmark::RegisterBenchmark("meow_fft", bm_meowfft);
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}

// vim:sw=2:ts=2:et
