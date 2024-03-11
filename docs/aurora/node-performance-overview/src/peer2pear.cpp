#include <cstdint>
#include <limits>
#include <mpi.h>
#include <random>
#include <sycl/sycl.hpp>
#include <vector>

void fill_randomly(sycl::queue Q, int N, std::vector<int *> ptrs) {
  std::vector<int> v(N);
  std::iota(v.begin(), v.end(), 0);

  std::minstd_rand g;
  for (auto &ptr : ptrs) {
    std::shuffle(v.begin(), v.end(), g);
    Q.memcpy(ptr, v.data(), N * sizeof(int)).wait();
  }
}

unsigned long datatransfer(int N, std::vector<std::pair<int, int *>> &sends,
                           std::vector<std::pair<int, int *>> &recvs) {

  unsigned long min_time = std::numeric_limits<unsigned long>::max();
  int num_iteration = 10;

  for (int r = 0; r < num_iteration; r++) {
    MPI_Barrier(MPI_COMM_WORLD);
    const unsigned long l_start =
        std::chrono::high_resolution_clock::now().time_since_epoch().count();

    std::vector<MPI_Request> requests;

    for (auto &[dest, ptr] : sends) {
      MPI_Request request = MPI_REQUEST_NULL;
      MPI_Isend(ptr, N, MPI_INT, dest, 0, MPI_COMM_WORLD, &request);
      requests.push_back(request);
    }

    for (auto &[src, ptr] : recvs) {
      MPI_Request request = MPI_REQUEST_NULL;
      MPI_Irecv(ptr, N, MPI_INT, src, 0, MPI_COMM_WORLD, &request);
      requests.push_back(request);
    }

    for (auto &request : requests)
      MPI_Wait(&request, MPI_STATUS_IGNORE);

    const unsigned long l_end =
        std::chrono::high_resolution_clock::now().time_since_epoch().count();
    unsigned long start, end;
    MPI_Reduce(&l_start, &start, 1, MPI_UNSIGNED_LONG, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&l_end, &end, 1, MPI_UNSIGNED_LONG, MPI_MAX, 0, MPI_COMM_WORLD);

    const unsigned long time = end - start;
    min_time = std::min(time, min_time);
  }
  return min_time;
}

int main() {
  MPI_Init(NULL, NULL);
  int world_size, world_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  sycl::queue Q(sycl::gpu_selector_v);
  const int N = 1 << 28;
  const int N_byte = N * sizeof(int);
  int *a_gpu = sycl::malloc_device<int>(N, Q);
  fill_randomly(Q, N, {a_gpu});

  std::vector<std::pair<int, int *>> sends;
  std::vector<std::pair<int, int *>> recvs;

  if (world_rank % 2 == 0)
    sends.push_back({world_rank + 1, a_gpu});
  else
    recvs.push_back({world_rank - 1, a_gpu});

  auto unitime = datatransfer(N, sends, recvs);
  if (world_rank == 0) {
    const double unitime_bw = (N_byte * world_size) / unitime;
    std::cout << "Tile2Tile Unidirectional: " << unitime_bw << "GB/s" << std::endl;
  }
  if (world_rank % 2 == 0)
    recvs.push_back({world_rank + 1, a_gpu});
  else
    sends.push_back({world_rank - 1, a_gpu});

  auto bitime = datatransfer(N, sends, recvs);
  if (world_rank == 0) {
    const double bitime_bw = (2L * N_byte * world_size) / bitime;
    std::cout << "Tile2Tile Bidirectional: " << bitime_bw << "GB/s" << std::endl;
  }
}
