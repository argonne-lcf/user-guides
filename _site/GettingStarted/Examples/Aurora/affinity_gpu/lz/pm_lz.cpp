#include <stdio.h>
#include <iostream>

#include "pm_lz.h"

int dev_num_devices()
{
  // should be in init() function
  zeInit(ZE_INIT_FLAG_GPU_ONLY);
  ze_driver_handle_t driverHandle;
  uint32_t driverCount = 1;
  zeDriverGet(&driverCount, &driverHandle);
 
  uint32_t deviceCount = 0;
  zeDeviceGet(driverHandle, &deviceCount, nullptr);
  // -- should be in init() function
 
  return deviceCount;
}

void dev_properties(int global_device_id)
{
  printf(" Printing short summary of platforms & devices\n");
  printf(" =============================================\n");
  
  // get vector of all available devices

  // -- should be in init() function
  ze_driver_handle_t driverHandle;
  uint32_t driverCount = 1;
  zeDriverGet(&driverCount, &driverHandle);

  uint32_t deviceCount = 0;
  zeDeviceGet(driverHandle, &deviceCount, nullptr);
  // -- should be in init() function

  ze_device_handle_t * device = nullptr;
  device = (ze_device_handle_t*) malloc(deviceCount * sizeof(ze_device_handle_t));
  zeDeviceGet(driverHandle, &deviceCount, device);

  // print useful information for each device

  int global_index = 0;
  for (int j=0; j<deviceCount; ++j) {
    printf("  Device %i: \n",j);
    ze_device_properties_t deviceProperties = {};
    zeDeviceGetProperties(device[j], &deviceProperties);

    uint32_t subDeviceCount = 0;
    zeDeviceGetSubDevices(device[j], &subDeviceCount, nullptr);
    uint32_t is_subdevice = (deviceProperties.flags & ZE_DEVICE_PROPERTY_FLAG_SUBDEVICE);
    printf("    subDeviceCount= %i   uuid= %lu  subdeviceId= %i  flags= %i  is_subdevice= %i\n",subDeviceCount, deviceProperties.uuid, deviceProperties.subdeviceId, deviceProperties.flags, is_subdevice);
    ze_device_handle_t subDevices[2] = {};
    zeDeviceGetSubDevices(device[j], &subDeviceCount, subDevices);
    for (uint32_t i = 0; i < subDeviceCount; i++) {
      ze_device_properties_t deviceProperties = {};
      zeDeviceGetProperties(subDevices[i], &deviceProperties);
      is_subdevice = (deviceProperties.flags & ZE_DEVICE_PROPERTY_FLAG_SUBDEVICE);
      printf("      [%i] uuid= %lu  subdeviceId= %i  flags= %i  is_subdevice= %i",i,deviceProperties.uuid, deviceProperties.subdeviceId, deviceProperties.flags, is_subdevice);

      if(global_index == global_device_id) {
        printf(" <--Selected\n");
      } else printf("\n");
      global_index++;
    }
   
  }

  printf("\n No more platforms or devices\n");
  free(device);
}
