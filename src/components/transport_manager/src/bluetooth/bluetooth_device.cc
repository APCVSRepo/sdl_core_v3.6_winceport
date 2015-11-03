/**
 * \file bluetooth_device.cc
 * \brief BluetoothDevice class source file.
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
#include "transport_manager/bluetooth/bluetooth_device.h"

#include <errno.h>
#ifdef OS_WINCE
#include "utils/global.h"
#else
#include <sys/types.h>
#endif
#ifdef OS_WIN32
#include <stdint.h>
#else
#include <sys/socket.h>
#endif

#include <algorithm>
#include <limits>

namespace transport_manager {
namespace transport_adapter {

bool BluetoothDevice::GetRfcommChannel(const ApplicationHandle app_handle,
                                       uint8_t* channel_out) {
	if (app_handle < 0 ||
#ifdef OS_WIN32
		app_handle > UINT8_MAX)
#else
		app_handle > std::numeric_limits<uint8_t>::max())
#endif
    return false;
  const uint8_t channel = static_cast<uint8_t>(app_handle);
  RfcommChannelVector::const_iterator it = std::find(rfcomm_channels_.begin(),
                                                     rfcomm_channels_.end(),
                                                     channel);
  if (it == rfcomm_channels_.end())
    return false;
  *channel_out = channel;
  return true;
}

#ifdef OS_WIN32
void BluetoothDevice::GetAddressString(const bdaddr_t& device_address, char *device_address_string, int address_size)
{
	DWORD addressSize = (DWORD)address_size;
	printf("GetAddressString(), addressSize=%d\n", addressSize);
	USHORT iNapAddress = GET_NAP(device_address.btAddr);
	ULONG iSapAddress = GET_SAP(device_address.btAddr);
	sprintf(device_address_string, "%02X:%02X:%02X:%02X:%02X:%02X", iNapAddress >> 8 & 0x00FF, iNapAddress & 0x00FF,
		iSapAddress >> 24 & 0x00FF, iSapAddress >> 16 & 0x00FF, iSapAddress >> 8 & 0x00FF, iSapAddress & 0x00FF);
	//DWORD ret = WSAAddressToString((LPSOCKADDR)&device_address, sizeof(bdaddr_t), NULL, device_address_string, &addressSize);
	//if (SOCKET_ERROR == ret)
	//{
	//	 DWORD dwError = GetLastError();
	//	 switch (dwError)
	//	 {
	//		 case WSAEFAULT:
	//		  printf("WSAEFAULT\n");
	//		  break;
	//		 case WSAEINVAL:
	//		  printf("WSAEINVAL\n");
	//		  break;
	//		 case WSAENOBUFS:
	//		  printf("WSAENOBUFS\n");
	//		  break;
	//		 case WSANOTINITIALISED:
	//		  printf("WSANOTINITIALISED\n");
	//		  break;
	//		 default:
	//		  break;
	//	 }
	//}
	printf("GetAddressString(), addressString=%s\n", device_address_string);
}

void BluetoothDevice::GetAddressInfo(const char *device_address_string, bdaddr_t& device_address)
{
	int sa_len;
#ifdef OS_WINCE
	wchar_string strOut;
	Global::toUnicode(device_address_string, CP_ACP, strOut);
	DWORD ret = WSAStringToAddress((wchar_t*)strOut.c_str(), AF_BTH, NULL, (LPSOCKADDR)(&device_address), &sa_len);
#else
	DWORD ret = WSAStringToAddress((char *)device_address_string, AF_BTH, NULL, (LPSOCKADDR)(&device_address), &sa_len);
#endif
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
}

#endif

std::string BluetoothDevice::GetUniqueDeviceId(const bdaddr_t& device_address) {
	char device_address_string[32];
#ifdef OS_WIN32
	memset(device_address_string, 0, sizeof(device_address_string));
	GetAddressString(device_address, device_address_string, sizeof(device_address_string));
#else
	ba2str(&device_address, device_address_string);
#endif
  return std::string("BT-") + device_address_string;
}

BluetoothDevice::BluetoothDevice(const bdaddr_t& device_address, const char* device_name,
                                 const RfcommChannelVector& rfcomm_channels)
    : Device(device_name, GetUniqueDeviceId(device_address)),
      address_(device_address),
      rfcomm_channels_(rfcomm_channels) {
}

bool BluetoothDevice::IsSameAs(const Device* other) const {
  bool result = false;

  const BluetoothDevice* other_bluetooth_device =
      dynamic_cast<const BluetoothDevice*>(other);

  if (0 != other_bluetooth_device) {
    if (0
        == memcmp(&address_, &other_bluetooth_device->address_,
                  sizeof(bdaddr_t))) {
      result = true;
    }
  }

  return result;
}

ApplicationList BluetoothDevice::GetApplicationList() const {
  return ApplicationList(rfcomm_channels_.begin(), rfcomm_channels_.end());
}

}  // namespace transport_adapter
}  // namespace transport_manager

