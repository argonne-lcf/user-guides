#include <cstdlib>
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
// Without arguments print plane connectivity (groups of GPU Tile direcly connected)
// Is argment X is passed, print the X Tiles.
int main(int argc, char **argv) {

  auto devices = sycl::device::get_devices(sycl::info::device_type::gpu);

  using d_sd_t = std::string;
  using connection_t = std::set<d_sd_t>;

  // Get Disjoint Connection
  std::unordered_map<zes_fabric_port_id_t, connection_t> h;
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
      std::string id = std::to_string(i) + "." + std::to_string(hProperties.subdeviceId);
      h[hProperties.portId].insert(id);

      zes_fabric_port_state_t hState;
      zesFabricPortGetState(hPort, &hState);
      h[hState.remotePortId].insert(id);
    }
  }
  std::set<connection_t> disjoin_connections;
  for (auto &[_, v] : h)
    disjoin_connections.insert(v);

  // Join Connection (assume fully connected plan)
  std::vector<connection_t> connections;
  for (auto &disjoin_connection : disjoin_connections) {
    for (auto &connection : connections)
      for (auto &dsd : disjoin_connection)
        if (connection.count(dsd) != 0) {
          connection.insert(disjoin_connection.begin(), disjoin_connection.end());
          goto next;
        }
    connections.push_back(disjoin_connection);
  next: {}
  }
  // Prints
  if (argc < 2) {
    for (auto &connection : connections) {
      for (auto &d : connection)
        std::cout << d << " ";
      std::cout << std::endl;
    }
  } else {
    std::vector<d_sd_t> flatten_connections;
    for (auto &connection : connections)
      flatten_connections.insert(flatten_connections.begin(), connection.begin(), connection.end());

    std::cout << flatten_connections[atoi(argv[1])] << std::endl;
  }
}
