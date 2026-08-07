#include <sycl/sycl.hpp>
#include <numeric>
#include <chrono>
#include <vector>
#include <iostream>
#include <cstdlib>   // for std::getenv

#define N (1 << 20)   // 1M floats = 4MB
#define REPEAT 1000

using namespace sycl;

int main() {
  const char* dev_env = std::getenv("ZE_FLAT_DEVICE_HIERARCHY");

  std::vector<sycl::device> devices{};
  std::vector<sycl::queue> queues{};
  sycl::platform plt{sycl::gpu_selector_v};
  auto devs = plt.get_devices();

  if (dev_env && std::string(dev_env) == "COMPOSITE") {
    for (auto &dev : devs) {
      auto sub_devices = dev.create_sub_devices<sycl::info::partition_property::partition_by_affinity_domain>(sycl::info::partition_affinity_domain::next_partitionable);
      for (auto sub_device : sub_devices) {
        devices.emplace_back( sub_device );
      }
    }
  } else if (dev_env && std::string(dev_env) == "FLAT") {
    devices = std::move(devs);
  }

  auto global_context = sycl::context(devices);
  for (int i=0; i<devices.size(); i++) {
    queues.emplace_back( sycl::queue(global_context, devices[i], sycl::property_list{sycl::property::queue::in_order{}}) );
  }

  if (devices.size() < 2) {
    std::cout << "Need at least 2 GPUs for P2P test.\n";
    return 0;
  }
  std::cout << "Detected " << devices.size() << " GPUs\n";

  float *buf0 = malloc_device<float>(N, queues[0]);
  // Initialize GPU 0's buffer with known values
  std::vector<float> host_data(N);
  std::iota(host_data.begin(), host_data.end(), 0.0f);
  queues[0].memcpy(buf0, host_data.data(), N * sizeof(float)).wait();

  std::cout << ".......{WRITE} GPU 0 to GPU <*>......." << std::endl;
  for (size_t peer = 1; peer < devices.size(); ++peer) {
    auto& dev0 = devices[0];
    auto& dev_peer = devices[peer];

    // Check P2P access support
    if (!dev0.ext_oneapi_can_access_peer(dev_peer, ext::oneapi::peer_access::access_supported)) {
      std::cout << "GPU 0 <-> GPU " << peer << ": P2P access NOT supported.\n";
      continue;
    }
    // Enable peer access
    dev0.ext_oneapi_enable_peer_access(dev_peer);

    // Allocate buffer on peer device
    float* buf_peer = malloc_device<float>(N, queues[peer]);

    // Warm-up copy
    queues[0].memcpy(buf_peer, buf0, N * sizeof(float)).wait();

    // Timed copy loop
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < REPEAT; ++i)
      queues[0].memcpy(buf_peer, buf0, N * sizeof(float));
    queues[0].wait();  // Wait for all copies to complete
    auto end = std::chrono::high_resolution_clock::now();

    double duration = std::chrono::duration<double>(end - start).count(); // seconds
    double total_bytes = static_cast<double>(REPEAT) * N * sizeof(float);
    double bw_gbps = total_bytes / duration / 1e9;
    std::cout << "GPU 0 -> GPU " << peer << ": Bandwidth = "
              << bw_gbps << " GB/s\n";

    sycl::free(buf_peer, queues[peer]);
  }

  std::cout << ".......{READ} GPU<*> to GPU 0......." << std::endl;
  for (size_t peer = 1; peer < devices.size(); ++peer) {
    auto& dev0 = devices[0];
    auto& dev_peer = devices[peer];

    // Check P2P access support
    if (!dev0.ext_oneapi_can_access_peer(dev_peer, ext::oneapi::peer_access::access_supported)) {
      std::cout << "GPU 0 <-> GPU " << peer << ": P2P access NOT supported.\n";
      continue;
    }
    // Enable peer access
    dev0.ext_oneapi_enable_peer_access(dev_peer);

    // Allocate buffer on peer device
    float* buf_peer = malloc_device<float>(N, queues[peer]);

    // Warm-up copy
    queues[0].memcpy(buf0, buf_peer, N * sizeof(float)).wait();

    // Timed copy loop
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < REPEAT; ++i)
      queues[0].memcpy(buf0, buf_peer, N * sizeof(float));
    queues[0].wait();  // Wait for all copies to complete
    auto end = std::chrono::high_resolution_clock::now();

    double duration = std::chrono::duration<double>(end - start).count(); // seconds
    double total_bytes = static_cast<double>(REPEAT) * N * sizeof(float);
    double bw_gbps = total_bytes / duration / 1e9;
    std::cout << "GPU " << peer << " -> GPU 0: Bandwidth = "
              << bw_gbps << " GB/s\n";

    sycl::free(buf_peer, queues[peer]);
  }

  sycl::free(buf0, queues[0]);
  return 0;
}
