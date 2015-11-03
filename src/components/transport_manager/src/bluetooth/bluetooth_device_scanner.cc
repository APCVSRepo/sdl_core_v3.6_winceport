/**
 * \file bluetooth_device_scanner.cc
 * \brief BluetoothDeviceScanner class header file.
 *
 * Copyright (c) 2013, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef MODIFY_FUNCTION_SIGN
#include <global_first.h>
#endif
#include "transport_manager/bluetooth/bluetooth_device_scanner.h"

#ifdef MODIFY_FUNCTION_SIGN
#ifdef OS_WIN32
#include "utils/global.h"
#else
//
#endif
#endif
#ifdef OS_WIN32
#ifdef OS_WINCE
#include <bthapi.h>
#include <bt_sdp.h>
#include <bt_api.h>
#else
#include <BluetoothAPIs.h>
#endif
#else
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/rfcomm.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#endif
#include "transport_manager/bluetooth/bluetooth_transport_adapter.h"
#ifdef OS_WIN32
#else
#include "transport_manager/bluetooth/bluetooth_device.h"
#endif

#include "utils/logger.h"
#ifdef OS_WIN32
#ifdef OS_WINCE
#pragma comment(lib,"ws2.lib")
#else
#pragma comment(lib,"ws2_32.lib")
#endif
#endif

namespace transport_manager {
namespace transport_adapter {

CREATE_LOGGERPTR_GLOBAL(logger_, "TransportManager")

namespace {
char* SplitToAddr(char* dev_list_entry) {
  char* bl_address = strtok(dev_list_entry, "()");
  if (bl_address != NULL) {
    bl_address = strtok(NULL, "()");
    return bl_address;
  } else {
    return NULL;
  }
}

#ifdef OS_WIN32
#else
int FindPairedDevs(std::vector<bdaddr_t>* result) {
  DCHECK(result != NULL);

  const char* cmd = "bt-device -l";

  FILE* pipe = popen(cmd, "r");
  if (!pipe) {
    return -1;
  }
  char* buffer = new char[1028];
  size_t counter = 0;
  while (fgets(buffer, 1028, pipe) != NULL) {
    if (0 < counter++) {  //  skip first line
      char* bt_address = SplitToAddr(buffer);
      if (bt_address) {
        bdaddr_t address;
        str2ba(bt_address, &address);
        result->push_back(address);
      }
    }
    delete [] buffer;
    buffer = new char[1028];
  }
#ifdef MODIFY_FUNCTION_SIGN
  if(buffer != NULL){
    delete[] buffer;
    buffer = NULL;
  }
#endif
  pclose(pipe);
  return 0;
}
#endif
}  //  namespace

BluetoothDeviceScanner::BluetoothDeviceScanner(
  TransportAdapterController* controller, bool auto_repeat_search,
  int auto_repeat_pause_sec)
  : controller_(controller),
    thread_(),
    thread_started_(false),
    shutdown_requested_(false),
    ready_(true),
    device_scan_requested_(false),
    device_scan_requested_lock_(),
    device_scan_requested_cv_(),
    auto_repeat_search_(auto_repeat_search),
    auto_repeat_pause_sec_(auto_repeat_pause_sec) {
#ifdef OS_WIN32
	// Initial winsock  
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		LOG4CXX_INFO(logger_, "WSAStartup failed with error code: " << WSAGetLastError());
	}
#endif
#ifdef OS_WIN32
	smart_device_link_service_uuid_.Data1 = 0x936da01f;
	smart_device_link_service_uuid_.Data2 = 0x9abd;
	smart_device_link_service_uuid_.Data3 = 0x4d9d;
	smart_device_link_service_uuid_.Data4[0] = 0x80;
	smart_device_link_service_uuid_.Data4[1] = 0xc7;
	smart_device_link_service_uuid_.Data4[2] = 0x02;
	smart_device_link_service_uuid_.Data4[3] = 0xaf;
	smart_device_link_service_uuid_.Data4[4] = 0x85;
	smart_device_link_service_uuid_.Data4[5] = 0xc8;
	smart_device_link_service_uuid_.Data4[6] = 0x22;
	smart_device_link_service_uuid_.Data4[7] = 0xa8;
#else
	uint8_t smart_device_link_service_uuid_data[] = { 0x93, 0x6D, 0xA0, 0x1F,
		0x9A, 0xBD, 0x4D, 0x9D, 0x80, 0xC7, 0x02, 0xAF, 0x85, 0xC8, 0x22, 0xA8
	};
	sdp_uuid128_create(&smart_device_link_service_uuid_,
		smart_device_link_service_uuid_data);
#endif
}

BluetoothDeviceScanner::~BluetoothDeviceScanner() {
#ifdef OS_WIN32
	WSACleanup();
#endif
}

void* bluetoothDeviceScannerThread(void* data) {
  LOG4CXX_TRACE_ENTER(logger_);
  BluetoothDeviceScanner* bluetoothDeviceScanner =
    static_cast<BluetoothDeviceScanner*>(data);
  assert(bluetoothDeviceScanner != 0);
  bluetoothDeviceScanner->Thread();
  LOG4CXX_TRACE_EXIT(logger_);
  return 0;
}

bool BluetoothDeviceScanner::IsInitialised() const {
  return thread_started_;
}

void BluetoothDeviceScanner::UpdateTotalDeviceList() {
  DeviceVector devices;
  devices.insert(devices.end(), paired_devices_with_sdl_.begin(),
                 paired_devices_with_sdl_.end());
  devices.insert(devices.end(), found_devices_with_sdl_.begin(),
                 found_devices_with_sdl_.end());
  controller_->SearchDeviceDone(devices);
}

void BluetoothDeviceScanner::DoInquiry() {
  LOG4CXX_TRACE_ENTER(logger_);

#ifdef OS_WIN32
  const int device_id = 0;
  int device_handle = 0;
#else
  const int device_id = hci_get_route(0);
  if (device_id < 0) {
#ifdef MODIFY_FUNCTION_SIGN
    LOG4CXX_INFO(logger_, "HCI device is not available" << " device_id " << device_id << "; errno " << errno);
#else
		LOG4CXX_INFO(logger_, "HCI device is not available");
#endif
    shutdown_requested_ = true;
    controller_->SearchDeviceFailed(SearchDeviceError());
    return;
  }

  int device_handle = hci_open_dev(device_id);
  if (device_handle < 0) {
    LOG4CXX_INFO(logger_, "device_handle < 0, exit DoInquiry");
    controller_->SearchDeviceFailed(SearchDeviceError());
    return;
  }

  if (paired_devices_.empty()) {
    LOG4CXX_INFO(logger_, "Searching for paired devices.");
    if (-1 == FindPairedDevs(&paired_devices_)) {
      LOG4CXX_ERROR(logger_, "Failed to retrieve list of paired devices.");
      controller_->SearchDeviceFailed(SearchDeviceError());
    }
  }

  LOG4CXX_INFO(logger_, "Check rfcomm channel on "
               << paired_devices_.size() << " paired devices.");

  paired_devices_with_sdl_.clear();
  CheckSDLServiceOnDevices(paired_devices_, device_handle,
                           &paired_devices_with_sdl_);
  UpdateTotalDeviceList();

  LOG4CXX_INFO(logger_, "Starting hci_inquiry on device " << device_id);
#endif

#ifdef OS_WIN32
  int number_of_devices = 0;
  std::vector < bdaddr_t > found_devices;
  // Initialize bluetooth struct of enumeration
  DWORD dwWsaqsLen = sizeof(WSAQUERYSET);
  LPWSAQUERYSET lpWsaqs = (LPWSAQUERYSET)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwWsaqsLen);
  lpWsaqs->dwSize = sizeof(WSAQUERYSET);
  lpWsaqs->dwNameSpace = NS_BTH;

  // Begin enumerate bluetooth device  
  DWORD dwWsaqsFlags = LUP_CONTAINERS | LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_ADDR;
  HANDLE hService;

  if (WSALookupServiceBegin(lpWsaqs, dwWsaqsFlags, &hService) != SOCKET_ERROR)
  {
	  bool bFinished = false;
	  while (!bFinished)
	  {
		  if (WSALookupServiceNext(hService, dwWsaqsFlags, &dwWsaqsLen, lpWsaqs) == NO_ERROR)
		  {
			  bdaddr_t *saBth = (bdaddr_t*)lpWsaqs->lpcsaBuffer->RemoteAddr.lpSockaddr;
			  BTH_ADDR bthAddr = saBth->btAddr;
			  //SOCKET_ADDRESS addr;
			  //memcpy(&addr, saBth, sizeof(SOCKADDR));
			  printf("\nFind BT Device: \n");
			  printf("------------------------\n");
			  printf("NAP: 0x%04X  SAP: 0x%08X DeviceName: %s ServerClassId.Data1: %d\n", GET_NAP(bthAddr), GET_SAP(bthAddr), lpWsaqs->lpszServiceInstanceName, saBth->serviceClassId.Data1);
#ifdef OS_WINCE
			  if (wcscmp(lpWsaqs->lpszServiceInstanceName, L"") != 0){
				  std::string strServiceInstanceName;
				  Global::fromUnicode(lpWsaqs->lpszServiceInstanceName, CP_ACP, strServiceInstanceName);
				  map_found_devices_.insert(std::pair<BTH_ADDR, std::string>(bthAddr, strServiceInstanceName));
#else
			  if (strcmp(lpWsaqs->lpszServiceInstanceName, "") != 0){
				  map_found_devices_.insert(std::pair<BTH_ADDR, std::string>(bthAddr, lpWsaqs->lpszServiceInstanceName));
#endif
				  found_devices.push_back(*saBth);
				  number_of_devices++;
			  }
		  }
		  else
		  {
			  // Error processing  
			  switch (WSAGetLastError())
			  {
				  // The buffer is too small, reapply for
			  case WSAEFAULT:
				  printf("\nFind BT Device: WSAEFAULT\n");
				  HeapFree(GetProcessHeap(), 0, lpWsaqs);
				  lpWsaqs = (LPWSAQUERYSET)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwWsaqsLen);
				  lpWsaqs->dwSize = sizeof(WSAQUERYSET);
				  lpWsaqs->dwNameSpace = NS_BTH;
				  break;

				  // No more bluetooth devices
			  case WSA_E_NO_MORE:
				  printf("\nFind BT Device: WSA_E_NO_MORE\n");
				  bFinished = true;
				  break;

			  default:
				  printf("\nFind BT Device: default\n");
				  //bFinished = true;  
				  break;
			  }
		  }
	  }
	  WSALookupServiceEnd(hService);
  }
  else
  {
	  Sleep(1000);
	  printf("\nFind BT Device Begn: \n");
	  DWORD dwerror = WSAGetLastError();
	  switch (dwerror)
	  {
	  case WSA_NOT_ENOUGH_MEMORY:
		  printf("WSA_NOT_ENOUGH_MEMORY\n");
		  break;
	  case WSAEINVAL:
		  printf("WSAEINVAL");
		  break;
	  case WSANO_DATA:
		  printf("WSANO_DATA");
		  break;
	  case WSANOTINITIALISED:
		  printf("WSANOTINITIALISED");
		  break;
	  case WSASERVICE_NOT_FOUND:
		  printf("WSASERVICE_NOT_FOUND");
		  break;
	  default:
		  break;
	  }
  }
  // Release resources of bluetooth
  HeapFree(GetProcessHeap(), 0, lpWsaqs);
#else
  const uint8_t inquiry_time = 8u;  // Time unit is 1.28 seconds
  const size_t max_devices = 256u;
  inquiry_info* inquiry_info_list = new inquiry_info[max_devices];

  const int number_of_devices = hci_inquiry(device_id, inquiry_time,
                                max_devices, 0, &inquiry_info_list,
                                IREQ_CACHE_FLUSH);
#endif
  if (number_of_devices >= 0) {
    LOG4CXX_INFO(logger_,
                 "hci_inquiry: found " << number_of_devices << " devices");
#ifdef OS_WIN32
#else
	std::vector < bdaddr_t > found_devices(number_of_devices);
	for (int i = 0; i < number_of_devices; ++i) {
		found_devices[i] = inquiry_info_list[i].bdaddr;
	}
#endif
    found_devices_with_sdl_.clear();
    CheckSDLServiceOnDevices(found_devices, device_handle,
                             &found_devices_with_sdl_);
  }
  UpdateTotalDeviceList();

#ifdef OS_WIN32
#else
  close(device_handle);
  delete[] inquiry_info_list;
#endif

  if (number_of_devices < 0) {
    LOG4CXX_INFO(logger_, "number_of_devices < 0");
    LOG4CXX_TRACE_EXIT(logger_);
    controller_->SearchDeviceFailed(SearchDeviceError());
  }

  LOG4CXX_INFO(logger_, "Bluetooth inquiry finished");
  LOG4CXX_TRACE_EXIT(logger_);
}

void BluetoothDeviceScanner::CheckSDLServiceOnDevices(
  const std::vector<bdaddr_t>& bd_addresses, int device_handle,
  DeviceVector* discovered_devices) {
  std::vector<RfcommChannelVector> sdl_rfcomm_channels =
    DiscoverSmartDeviceLinkRFCOMMChannels(bd_addresses);

  for (size_t i = 0; i < bd_addresses.size(); ++i) {
    if (sdl_rfcomm_channels[i].empty()) {
      continue;
    }

    const bdaddr_t& bd_address = bd_addresses[i];
    char deviceName[256];
#ifdef OS_WIN32
	memset(deviceName, 0, sizeof(deviceName));
	strcpy(deviceName, map_found_devices_[bd_address.btAddr].c_str());
#else
    int hci_read_remote_name_ret = hci_read_remote_name(
                                     device_handle, &bd_address, sizeof(deviceName) / sizeof(deviceName[0]),
                                     deviceName, 0);

    if (hci_read_remote_name_ret != 0) {
      LOG4CXX_ERROR_WITH_ERRNO(logger_, "hci_read_remote_name failed");
      strncpy(deviceName,
              BluetoothDevice::GetUniqueDeviceId(bd_address).c_str(),
              sizeof(deviceName) / sizeof(deviceName[0]));
    }
#endif

#ifdef MODIFY_FUNCTION_SIGN
#ifdef OS_WIN32
		std::string strSrc(deviceName);
		std::string strOut;
		Global::anyMultiToUtf8Multi(strSrc, strOut);
		printf("new bluetoothDevice(bd_address, %s)\n", strOut.c_str());
		Device* bluetooth_device = new BluetoothDevice(bd_address, strOut.c_str(),
			sdl_rfcomm_channels[i]);
#else
		Device* bluetooth_device = new BluetoothDevice(bd_address, deviceName,
        sdl_rfcomm_channels[i]);
#endif
#else
    Device* bluetooth_device = new BluetoothDevice(bd_address, deviceName,
        sdl_rfcomm_channels[i]);
#endif
    if (bluetooth_device) {
      LOG4CXX_INFO(logger_, "Bluetooth device created successfully");
      discovered_devices->push_back(bluetooth_device);
    } else {
      LOG4CXX_WARN(logger_, "Can't create bluetooth device " << deviceName);
    }
  }
#ifdef OS_WIN32
  map_found_devices_.clear();
#endif
}

std::vector<BluetoothDeviceScanner::RfcommChannelVector> BluetoothDeviceScanner::DiscoverSmartDeviceLinkRFCOMMChannels(
  const std::vector<bdaddr_t>& device_addresses) {
  LOG4CXX_TRACE_ENTER(logger_);
  const size_t size = device_addresses.size();
  std::vector<RfcommChannelVector> result(size);

#ifdef OS_WIN32
  static const int attempts = 1;
  static const int attempt_timeout = 1;
#else
  static const int attempts = 4;
  static const int attempt_timeout = 5;
#endif
  std::vector<bool> processed(size, false);
  unsigned processed_count = 0;
  for (int nattempt = 0; nattempt < attempts; ++nattempt) {
    for (size_t i = 0; i < size; ++i) {
      if (processed[i]) {
        continue;
      }
      const bool final = DiscoverSmartDeviceLinkRFCOMMChannels(
                           device_addresses[i], &result[i]);
      if (final) {
        processed[i] = true;
        ++processed_count;
      }
    }
    if (++processed_count >= size) {
      break;
    }
#ifdef OS_WIN32
	Sleep(attempt_timeout * 1000);
#else
	sleep(attempt_timeout);
#endif
  }
  return result;
}

bool BluetoothDeviceScanner::DiscoverSmartDeviceLinkRFCOMMChannels(
  const bdaddr_t& device_address, RfcommChannelVector* channels) {
#ifdef OS_WIN32
	// Get rfcomm channels...
#ifdef OS_WINCE
	  wchar_t				addressAsString[64];
#else
	char                    addressAsString[64];
#endif
	int                     retvalue = 0;
	HANDLE                  hLookup2;
	WSAQUERYSET             *pretvalueStruct;
	WSAPROTOCOL_INFO        protocolInfo;
	int						protocolInfoSize;
	DWORD					dwWsaqsFlags;
	CSADDR_INFO*			pCSAddr;

	SOCKET serach_socket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);

	/* Set socket options */
	protocolInfoSize = sizeof(protocolInfo);
	if (0 != getsockopt(serach_socket, SOL_SOCKET, SO_PROTOCOL_INFO, (char*)&protocolInfo, &protocolInfoSize))
	{
		return false;
	}

	DWORD dwBegFlags = /*LUP_CONTAINERS;//*/  LUP_FLUSHCACHE;
	/* set flags and pointers */
	dwWsaqsFlags = LUP_FLUSHCACHE | LUP_RETURN_NAME | LUP_RETURN_TYPE | LUP_RETURN_ADDR | LUP_RETURN_BLOB | LUP_RETURN_COMMENT;
	WSAQUERYSET querySet2;
	/* Initialize queryset2 */
	memset(&querySet2, 0, sizeof(querySet2));
	querySet2.dwSize = sizeof(querySet2);
	querySet2.lpServiceClassId = &smart_device_link_service_uuid_;
	querySet2.dwNameSpace = NS_BTH;
	memset(addressAsString, 0, sizeof(addressAsString));
	DWORD addressSize = sizeof(addressAsString);


	//if (0 == WSAAddressToString(pCSAddr->RemoteAddr.lpSockaddr, pCSAddr->RemoteAddr.iSockaddrLength, &protocolInfo, addressAsString, &addressSize)){
	//	printf("Query condition addressAsString = %s\n", addressAsString);
	//}
	//	USHORT iNapAddress = GET_NAP(device_address);
	//	ULONG iSapAddress = GET_SAP(device_address);
	//	sprintf(addressAsString, "(%2X:%2X:%2X:%2X:%2X:%2X)", iNapAddress >> 8 & 0x00FF, iNapAddress & 0x00FF,
	//		iSapAddress >> 24 & 0x00FF, iSapAddress >> 16 & 0x00FF, iSapAddress >> 8 & 0x00FF, iSapAddress & 0x00FF);
	//	printf("Query condition addressAsString = %s\n", addressAsString);

	/* Set query starting from current device */
	DWORD dwLen;
	printf("DiscoverSmartDeviceLinkRFCOMMChannels()'WSAAddressToString()'s ");
	DWORD ret = WSAAddressToString((LPSOCKADDR)&device_address, sizeof(bdaddr_t), NULL, addressAsString, &addressSize);
	if (SOCKET_ERROR == ret)
	{
		DWORD dwError = GetLastError();
		switch (dwError)
		{
		case WSAEFAULT:
			printf("WSAEFAULT\n");
			break;
		case WSAEINVAL:
			printf("WSAEINVAL\n");
			break;
		case WSAENOBUFS:
			printf("WSAENOBUFS\n");
			break;
		case WSANOTINITIALISED:
			printf("WSANOTINITIALISED\n");
			break;
		default:
			break;
		}
	}
	printf("addressAsString:%s\n", addressAsString);
	querySet2.lpszContext = addressAsString;
	/* Start to search services */
	retvalue = WSALookupServiceBegin(&querySet2, dwBegFlags, &hLookup2);

	/* Is there services  */
	if (0 != retvalue){
		DWORD dwError = GetLastError();
		switch (dwError)
		{
		case WSA_NOT_ENOUGH_MEMORY:
			printf("WSA_NOT_ENOUGH_MEMORY\n");
			break;
		case WSAEINVAL:
			printf("WSAEINVAL\n");
			break;
		case WSANO_DATA:
			printf("WSANO_DATA\n");
			break;
		case WSANOTINITIALISED:
			printf("WSANOTINITIALISED\n");
			break;
		case WSASERVICE_NOT_FOUND:
			printf("WSASERVICE_NOT_FOUND\n");
			break;
		default:
			break;
		}
		printf("WSALookupServiceBegin querySet2 fail, return value is %d\n", retvalue);
		return false;
	}
	else{
		while (0 == retvalue)
		{
			/* Initialize buffer for searching */
			BYTE buffer[2048];
			DWORD bufferLength = sizeof(buffer);
			pretvalueStruct = (WSAQUERYSET*)&buffer;
			memset(&buffer, 0, sizeof(buffer));
			pretvalueStruct->dwSize = sizeof(WSAQUERYSET);

			retvalue = WSALookupServiceNext(hLookup2, dwWsaqsFlags, &bufferLength, pretvalueStruct);

			if (retvalue == 0)
			{
				pCSAddr = (CSADDR_INFO *)pretvalueStruct->lpcsaBuffer;
				addressSize = sizeof(addressAsString);
				memset(&addressAsString, 0, sizeof(addressAsString));

				if (pCSAddr == 0)
					printf("\n====================================================================\nthe pCSaddr is null\n");

				/* Check that pCSAddr is not empty, so print channel*/
				if (pCSAddr != 0)
				{
					if (0 == WSAAddressToString(pCSAddr->RemoteAddr.lpSockaddr, pCSAddr->RemoteAddr.iSockaddrLength, &protocolInfo, addressAsString, &addressSize))
					{
						printf("\n==================================\nRemote Protocol = %d\n", pretvalueStruct->lpcsaBuffer->iProtocol);
						printf("Query result addressAsString = %s\n", addressAsString);
						int protocol = pretvalueStruct->lpcsaBuffer->iProtocol;
						/* If channel number is too large don't print it */
						//if (addressAsString[44] == 0)
						//{
						/* RFCOMM: 0x0003 */
						/* L2CAP : 0x0100 */

						if (protocol == 0x0003)
							printf("\nRFCOMM:channel:");
						else
						if (protocol == 0x0100)
							printf("\nL2CAP:PSM:");
						else
							printf("\n[UNKNOWN]:");

						/* channel number */
#ifdef OS_WINCE
						std::string strAddressString;
						Global::fromUnicode(addressAsString, CP_ACP, strAddressString);
#else
						std::string strAddressString = addressAsString;
#endif
						int nPortPos = (int)strAddressString.rfind(':');
						if (nPortPos >= 0){
							strAddressString = strAddressString.substr(nPortPos + 1);
							int nChannel = atoi(strAddressString.c_str());
							channels->push_back(nChannel);
							printf("%d\n", nChannel);
						}
						//}
					}
					else
						printf("\n====================================================================\naddress to string is wrong\n");

				}
			}
			else
			{
				printf("WSALookupServiceNext faled\n");
				DWORD dwError = GetLastError();
				switch (dwError)
				{
				case WSA_E_CANCELLED:
					printf("WSA_E_CANCELLED\n");
					break;
				case WSA_E_NO_MORE:
					printf("WSA_E_NO_MORE\n");
					break;
				case WSAEFAULT:
					printf("WSAEFAULT\n");
					break;
				case WSAEINVAL:
					printf("WSAEINVAL\n");
					break;
				case WSA_INVALID_HANDLE:
					printf("WSA_INVALID_HANDLE\n");
					break;
				case WSANOTINITIALISED:
					printf("WSANOTINITIALISED\n");
					break;
				case WSANO_DATA:
					printf("WSANO_DATA\n");
					break;
				case WSA_NOT_ENOUGH_MEMORY:
					printf("WSA_NOT_ENOUGH_MEMORY\n");
					break;
				default:
					break;
				}
			}
		} /* while service searching loop */

		retvalue = WSALookupServiceEnd(hLookup2);
		return true;
	}
#else
  static bdaddr_t any_address = { { 0, 0, 0, 0, 0, 0 } };

  sdp_session_t* sdp_session = sdp_connect(
                                 &any_address, &device_address, SDP_RETRY_IF_BUSY | SDP_WAIT_ON_CLOSE);
  if (sdp_session == 0) {
    LOG4CXX_INFO(logger_, "sdp_connect errno " << errno);
    return !(errno == 31 || errno == 16 || errno == 117 || errno == 114);
  }

  sdp_list_t* search_list = sdp_list_append(0,
                            &smart_device_link_service_uuid_);
  uint32_t range = 0x0000ffff;
  sdp_list_t* attr_list = sdp_list_append(0, &range);
  sdp_list_t* response_list = 0;

  if (0
      == sdp_service_search_attr_req(sdp_session, search_list,
                                     SDP_ATTR_REQ_RANGE, attr_list,
                                     &response_list)) {
    for (sdp_list_t* r = response_list; 0 != r; r = r->next) {
      sdp_record_t* sdp_record = static_cast<sdp_record_t*>(r->data);
      sdp_list_t* proto_list = 0;

      if (0 == sdp_get_access_protos(sdp_record, &proto_list)) {
        for (sdp_list_t* p = proto_list; 0 != p; p = p->next) {
          sdp_list_t* pdsList = static_cast<sdp_list_t*>(p->data);

          for (sdp_list_t* pds = pdsList; 0 != pds; pds = pds->next) {
            sdp_data_t* sdpData = static_cast<sdp_data_t*>(pds->data);
            int proto = 0;

            for (sdp_data_t* d = sdpData; 0 != d; d = d->next) {
              switch (d->dtd) {
                case SDP_UUID16:
                case SDP_UUID32:
                case SDP_UUID128:
                  proto = sdp_uuid_to_proto(&d->val.uuid);
                  break;

                case SDP_UINT8:
                  if (RFCOMM_UUID == proto) {
                    channels->push_back(d->val.uint8);
                  }
                  break;
              }
            }
          }

          sdp_list_free(pdsList, 0);
        }

        sdp_list_free(proto_list, 0);
      }
    }
  }
  sdp_list_free(search_list, 0);
  sdp_list_free(attr_list, 0);
  sdp_list_free(response_list, 0);
  sdp_close(sdp_session);

  if (!channels->empty()) {
    LOG4CXX_INFO(logger_, "channels not empty");
    std::stringstream rfcomm_channels_string;

    for (RfcommChannelVector::const_iterator it = channels->begin();
         it != channels->end(); ++it) {
      if (it != channels->begin()) {
        rfcomm_channels_string << ", ";
      }
      rfcomm_channels_string << static_cast<uint32_t>(*it);
    }

    LOG4CXX_INFO(
      logger_,
      "SmartDeviceLink service was discovered on device "
      << BluetoothDevice::GetUniqueDeviceId(device_address)
      << " at channel(s): " << rfcomm_channels_string.str().c_str());
  } else {
    LOG4CXX_INFO(
      logger_,
      "SmartDeviceLink service was not discovered on device "
      << BluetoothDevice::GetUniqueDeviceId(device_address));
  }
  LOG4CXX_TRACE_EXIT(logger_);
  return true;
#endif
}

void BluetoothDeviceScanner::Thread() {
  LOG4CXX_TRACE_ENTER(logger_);
  LOG4CXX_INFO(logger_, "Bluetooth adapter main thread initialized");
  ready_ = true;
  if (auto_repeat_search_) {
    while (!shutdown_requested_) {
      DoInquiry();
      device_scan_requested_ = false;
      TimedWaitForDeviceScanRequest();
    }
  } else {  // search only on demand
    while (true) {
      {
        sync_primitives::AutoLock auto_lock(device_scan_requested_lock_);
        while (!(device_scan_requested_ || shutdown_requested_)) {
          device_scan_requested_cv_.Wait(auto_lock);
        }
      }
      if (shutdown_requested_) {
        break;
      }
      DoInquiry();
      device_scan_requested_ = false;
    }
  }

  LOG4CXX_INFO(logger_, "Bluetooth device scanner thread finished");
  LOG4CXX_TRACE_EXIT(logger_);
}

void BluetoothDeviceScanner::TimedWaitForDeviceScanRequest() {
  LOG4CXX_TRACE_ENTER(logger_);

  if (auto_repeat_pause_sec_ == 0) {
    LOG4CXX_TRACE_EXIT(logger_);
    return;
  }

  {
    sync_primitives::AutoLock auto_lock(device_scan_requested_lock_);
    while (!(device_scan_requested_ || shutdown_requested_)) {
      const sync_primitives::ConditionalVariable::WaitStatus wait_status =
          device_scan_requested_cv_.WaitFor(auto_lock, auto_repeat_pause_sec_ * 1000);
      if (wait_status == sync_primitives::ConditionalVariable::kTimeout) {
        LOG4CXX_INFO(logger_, "Bluetooth scanner timeout, performing scan");
        device_scan_requested_ = true;
      }
    }
  }

  LOG4CXX_TRACE_EXIT(logger_);
}

TransportAdapter::Error BluetoothDeviceScanner::Init() {
  LOG4CXX_TRACE_ENTER(logger_);
  const int thread_start_error = pthread_create(&thread_, 0,
                                 &bluetoothDeviceScannerThread,
                                 this);

  if (0 == thread_start_error) {
    thread_started_ = true;
    LOG4CXX_INFO(logger_, "Bluetooth device scanner thread started");
  } else {
    LOG4CXX_ERROR(
      logger_,
      "Bluetooth device scanner thread start failed, error code "
      << thread_start_error);
    LOG4CXX_TRACE_EXIT(logger_);
    return TransportAdapter::FAIL;
  }
  LOG4CXX_TRACE_EXIT(logger_);
  return TransportAdapter::OK;
}

void BluetoothDeviceScanner::Terminate() {
  LOG4CXX_TRACE_ENTER(logger_);
  shutdown_requested_ = true;

  if (true == thread_started_) {
    {
      sync_primitives::AutoLock auto_lock(device_scan_requested_lock_);
      device_scan_requested_ = false;
      device_scan_requested_cv_.NotifyOne();
    }
    LOG4CXX_INFO(logger_,
                 "Waiting for bluetooth device scanner thread termination");
    pthread_join(thread_, 0);
    LOG4CXX_INFO(logger_, "Bluetooth device scanner thread terminated");
  }
  LOG4CXX_TRACE_EXIT(logger_);
}

TransportAdapter::Error BluetoothDeviceScanner::Scan() {
  LOG4CXX_TRACE_ENTER(logger_);
  if ((!thread_started_) || shutdown_requested_) {
    LOG4CXX_INFO(logger_, "bad state");
    return TransportAdapter::BAD_STATE;
  }
  if (auto_repeat_pause_sec_ == 0) {
    LOG4CXX_INFO(logger_, "no search pause, scan forcing not needed");
    return TransportAdapter::OK;
  }
  TransportAdapter::Error ret = TransportAdapter::OK;

  {
    sync_primitives::AutoLock auto_lock(device_scan_requested_lock_);
    if (false == device_scan_requested_) {
      LOG4CXX_INFO(logger_, "Requesting device Scan");
      device_scan_requested_ = true;
      device_scan_requested_cv_.NotifyOne();
    } else {
      ret = TransportAdapter::BAD_STATE;
      LOG4CXX_INFO(logger_, "Device Scan is currently in progress");
    }
  }

  LOG4CXX_TRACE_EXIT(logger_);
  return ret;
}

}  // namespace transport_adapter
}  // namespace transport_manager

