#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <time.h>

#include <iomanip>
#include <vector>
#include <tuple>

//https://stackoverflow.com/questions/68823023/set-cuda-device-by-uuid
void uuid_print(cudaUUID_t a)
{
  std::cout << "GPU";
  std::vector<std::tuple<int, int> > r = {{0,4}, {4,6}, {6,8}, {8,10}, {10,16}};
  for (auto t : r){
    std::cout << "-";
    for (int i = std::get<0>(t); i < std::get<1>(t); i++)
      std::cout << std::hex << std::setfill('0') << std::setw(2) << (unsigned)(unsigned char)a.bytes[i];
  }
  std::cout << std::endl;
}

int gpu_num_devices()
{
  int n;
  cudaGetDeviceCount(&n);
  return n;
}

void gpu_print_summary(int rnk, int num_devices)
{
  for(int id=0; id<num_devices; ++id) {	
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, id);
    printf("    [%i,%i] Platform[ Nvidia ] Type[ GPU ] Device[ %s ]  uuid= ",rnk, id, prop.name);
    uuid_print(prop.uuid);
    printf("\n");
  }
}
