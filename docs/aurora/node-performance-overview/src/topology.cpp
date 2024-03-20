#include <iostream>
#include <level_zero/ze_api.h>
#include <level_zero/zes_api.h>
#include <set>
#include <sycl.hpp>
#include <unordered_map>
#include <vector>

template <> struct std::hash<zes_fabric_port_id_t> {
  std::size_t operator()(const zes_fabric_port_id_t &k) const {
    // Compute individual hash values for first, second and third
    // http://stackoverflow.com/a/1646913/126995
    std::size_t res = 17;
    res = res * 31 + hash<uint32_t>()(k.fabricId);
    res = res * 31 + hash<uint32_t>()(k.attachId);
    res = res * 31 + hash<uint8_t>()(k.portNumber);
    return res;
  }
};

bool operator==(const zes_fabric_port_id_t &lhs, const zes_fabric_port_id_t &rhs) {
  return lhs.fabricId == rhs.fabricId && lhs.attachId == rhs.attachId &&
         lhs.portNumber == rhs.portNumber;
};

int main() {

  auto devices = sycl::device::get_devices(sycl::info::device_type::gpu);

  using d_sd_t = std::pair<int, int>;
  std::unordered_map<zes_fabric_port_id_t, std::set<d_sd_t>> h;

  for (int i = 0; i < devices.size(); i++) {
    zes_device_handle_t hDevice =
        sycl::get_native<sycl::backend::ext_oneapi_level_zero>(devices[i]);
    uint32_t numPorts;
    zesDeviceEnumFabricPorts(hDevice, &numPorts, nullptr);
    std::vector<zes_fabric_port_handle_t> fp_handles(numPorts);
    zesDeviceEnumFabricPorts(hDevice, &numPorts, fp_handles.data());
    for (auto &hPort : fp_handles) {
      zes_fabric_port_properties_t hProperties;
      zesFabricPortGetProperties(hPort, &hProperties);
      h[hProperties.portId].insert({i, hProperties.subdeviceId});

      zes_fabric_port_state_t hState;
      zesFabricPortGetState(hPort, &hState);
      h[hState.remotePortId].insert({i, hProperties.subdeviceId});
    }
  }
  // Merge overlaping set
  std::set<std::set<d_sd_t>> disjoin_connections;
  for (auto &[_, v] : h) {
    disjoin_connections.insert(v);
  }
  std::vector<std::set<d_sd_t>> connections;
  for (auto &disjoin_connection : disjoin_connections) {
    for (auto &connection : connections)
      for (auto &d : disjoin_connection)
        if (connection.count(d) != 0) {
          connection.insert(disjoin_connection.begin(), disjoin_connection.end());
          goto next;
        }
    connections.push_back(disjoin_connection);
  next : {}
  }

  for (auto &connection : connections) {
    for (auto &[d, sd] : connection)
      std::cout << d << ":" << sd << " ";
    std::cout << std::endl;
  }
}
