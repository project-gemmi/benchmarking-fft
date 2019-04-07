// 3D transpose microbenchmark

#include <complex>
#include <cassert>
#include <benchmark/benchmark.h>

static int x;
static int y;
static int z;
using type = std::complex<float>;
static std::vector<type> input;

static int index(int i, int j, int k) {
  return i * y * z + j * z + k;
}

static void bm_assign(benchmark::State& state) {
  std::vector<type> output;
  while (state.KeepRunning()) {
    output = input;
    benchmark::DoNotOptimize(output);
  }
}

static void bm_naive_zyx(benchmark::State& state) {
  std::vector<type> output = input;
  while (state.KeepRunning()) {
    for (int i = 0; i < x; ++i)
      for (int j = 0; j < y; ++j)
        for (int k = 0; k < z; ++k) {
          int n1 = i * y * z + j * z + k;
          int n2 = i + j * x + k * y * x;
          output[n2] = input[n1];
        }
    benchmark::DoNotOptimize(output);
  }
}

static void bm_naive_xzy(benchmark::State& state) {
  std::vector<type> output = input;
  while (state.KeepRunning()) {
    for (int i = 0; i < x; ++i)
      for (int j = 0; j < y; ++j)
        for (int k = 0; k < z; ++k) {
          int n1 = i * y * z + j * z + k;
          int n2 = i * y * z + j + k * y;
          output[n2] = input[n1];
        }
    benchmark::DoNotOptimize(output);
  }
}

static void bm_naive_yxz(benchmark::State& state) {
  std::vector<type> output = input;
  while (state.KeepRunning()) {
    for (int i = 0; i < x; ++i)
      for (int j = 0; j < y; ++j)
        for (int k = 0; k < z; ++k) {
          int n1 = i * y * z + j * z + k;
          int n2 = i * z + j * x * z + k;
          output[n2] = input[n1];
        }
    benchmark::DoNotOptimize(output);
  }
}

// equivalent of 2D transpose
static void bm_naive_zxy(benchmark::State& state) {
  std::vector<type> output = input;
  while (state.KeepRunning()) {
    for (int i = 0; i < x; ++i)
      for (int j = 0; j < y; ++j)
        for (int k = 0; k < z; ++k) {
          int n1 = (i * y + j) * z + k;
          int n2 = i * y + j + k * x * y;
          output[n2] = input[n1];
        }
    benchmark::DoNotOptimize(output);
  }
}

static void bm_naive_yzx(benchmark::State& state) {
  std::vector<type> output = input;
  while (state.KeepRunning()) {
    for (int i = 0; i < x; ++i)
      for (int j = 0; j < y; ++j)
        for (int k = 0; k < z; ++k) {
          int n1 = i * y * z + j * z + k;
          int n2 = j * x * z + k * x + i;
          output[n2] = input[n1];
        }
    benchmark::DoNotOptimize(output);
  }
}

static void bm_tiled_zxy(benchmark::State& state) {
  std::vector<type> output = input;
  const int N = 8;
  while (state.KeepRunning()) {
    int xy = x * y;
    for (int i = 0; i < xy; i += N)
      for (int k = 0; k < z; k += N)
        for (int i2 = i; i2 < std::min(i + N, xy); ++i2)
          for (int k2 = k; k2 < std::min(k + N, z); ++k2) {
            int n1 = i2 * z + k2;
            int n2 = i2 + k2 * xy;
            output[n2] = input[n1];
          }
    benchmark::DoNotOptimize(output);
  }
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
  for (int i = 0; i < x; ++i)
    for (int j = 0; j < y; ++j)
      for (int k = 0; k < z; ++k)
        input[index(i, j, k)] = {0.3f * i + 0.1f * j * j - 0.1f * k * k,
                                 10.f * i - sqrtf(i)};
  benchmark::RegisterBenchmark("assign",    bm_assign);
  benchmark::RegisterBenchmark("naive zyx", bm_naive_zyx);
  benchmark::RegisterBenchmark("naive xzy", bm_naive_xzy);
  benchmark::RegisterBenchmark("naive yxz", bm_naive_yxz);
  benchmark::RegisterBenchmark("naive zxy", bm_naive_zxy);
  benchmark::RegisterBenchmark("naive yzx", bm_naive_yzx);
  benchmark::RegisterBenchmark("tiled zxy", bm_tiled_zxy);
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}

// vim:sw=2:ts=2:et
