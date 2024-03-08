#undef MAD_4
#undef MAD_16
#undef MAD_64

#define MAD_4(x, y)                                                                                \
  x = y * x + y;                                                                                   \
  y = x * y + x;                                                                                   \
  x = y * x + y;                                                                                   \
  y = x * y + x;
#define MAD_16(x, y)                                                                               \
  MAD_4(x, y);                                                                                     \
  MAD_4(x, y);                                                                                     \
  MAD_4(x, y);                                                                                     \
  MAD_4(x, y);
#define MAD_64(x, y)                                                                               \
  MAD_16(x, y);                                                                                    \
  MAD_16(x, y);                                                                                    \
  MAD_16(x, y);                                                                                    \
  MAD_16(x, y);

// Naive port of some portion of clpeak
// (https://github.com/krrishnarraj/clpeak/)
#include <iostream>
#include <limits>
#include <mpi.h>
#include <omp.h>
#include <vector>

int main(int argc, char **argv) {

  MPI_Init(NULL, NULL);
  int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  const int globalWI{2000000000};
  const int num_iteration{10};

  std::vector<double> A(globalWI);
  double *Aptr{A.data()};

#pragma omp target enter data map(alloc : Aptr[0 : globalWI])

  double min_time = std::numeric_limits<double>::max();
  for (int r = 0; r < num_iteration; r++) {
    MPI_Barrier(MPI_COMM_WORLD);
    const double l_start = omp_get_wtime();
#pragma omp target teams distribute parallel for
    for (int64_t i = 0; i < globalWI; i++) {
      double x = 1.000000001f;
      double y = (double)i;
      for (int j = 0; j < 128; j++) {
        MAD_16(x, y);
      }
      Aptr[i] = y;
    }
    const double l_end = omp_get_wtime();
    double start, end;
    MPI_Reduce(&l_start, &start, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&l_end, &end, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    const double time = end - start;
    min_time = std::min(time, min_time);
  }
#pragma omp target exit data map(from : Aptr[0 : globalWI])

  const double workPerWI{128 * 16 * 2}; // Indicates flops executed per work-item
  const double gflops = (workPerWI * globalWI * world_size * 1E-9) / min_time;
  if (world_rank == 0)
    std::cout << "gflops: " << gflops << std::endl;
}
