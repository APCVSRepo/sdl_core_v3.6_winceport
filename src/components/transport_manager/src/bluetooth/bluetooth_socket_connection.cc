/**
 * \file bluetooth_socket_connection.cc
 * \brief BluetoothSocketConnection class source file.
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
#include "transport_manager/bluetooth/bluetooth_socket_connection.h"

#include <unistd.h>
#ifdef OS_WIN32
#include <winsock2.h>  
#include <ws2bth.h>
#else
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/rfcomm.h>
#endif

#include "transport_manager/bluetooth/bluetooth_device.h"
#include "transport_manager/transport_adapter/transport_adapter_controller.h"

#include "utils/logger.h"

namespace transport_manager {
namespace transport_adapter {

CREATE_LOGGERPTR_GLOBAL(logger_, "TransportManager")

BluetoothSocketConnection::BluetoothSocketConnection(
    const DeviceUID& device_uid, const ApplicationHandle& app_handle,
    TransportAdapterController* controller)
    : ThreadedSocketConnection(device_uid, app_handle, controller) {
}

BluetoothSocketConnection::~BluetoothSocketConnection() {
}

bool BluetoothSocketConnection::Establish(ConnectError** error) {
#ifdef OS_WIN32
	LOG4CXX_INFO(logger_, "enter (#" << pthread_self().x << ")");
#else
	LOG4CXX_INFO(logger_, "enter (#" << pthread_self() << ")");
#endif
  DeviceSptr device = controller()->FindDevice(device_handle());

  BluetoothDevice* bluetooth_device =
      static_cast<BluetoothDevice*>(device.get());

  uint8_t rfcomm_channel;
  if (!bluetooth_device->GetRfcommChannel(application_handle(),
                                          &rfcomm_channel)) {
    LOG4CXX_ERROR(logger_,
                  "Application " << application_handle() << " not found");
    *error = new ConnectError();
#ifdef OS_WIN32
	LOG4CXX_INFO(logger_, "exit (#" << pthread_self().x << ")");
#else
	LOG4CXX_INFO(logger_, "exit (#" << pthread_self() << ")");
#endif
    return false;
  }

#ifdef OS_WIN32
  GUID guidServiceClass = { 0x936da01f, 0x9abd, 0x4d9d, { 0x80, 0xc7, 0x02, 0xaf, 0x85, 0xc8, 0x22, 0xa8 } };
  bdaddr_t remoteSocketAddress;
  remoteSocketAddress.addressFamily = AF_BTH;
  remoteSocketAddress.port = rfcomm_channel;
  remoteSocketAddress.serviceClassId = guidServiceClass;
  remoteSocketAddress.btAddr = bluetooth_device->address().btAddr;

  SOCKET rfcomm_socket;
#else
  struct sockaddr_rc remoteSocketAddress = { 0 };
  remoteSocketAddress.rc_family = AF_BLUETOOTH;
  memcpy(&remoteSocketAddress.rc_bdaddr, &bluetooth_device->address(),
         sizeof(bdaddr_t));
  remoteSocketAddress.rc_channel = rfcomm_channel;

  int rfcomm_socket;
#endif

#ifdef OS_WIN32
  int attempts = 1;
#else
  int attempts = 4;
#endif
  int connect_status = 0;
  LOG4CXX_INFO(logger_, "start rfcomm Connect attempts");
	printf("start rfcomm Connect attempts\n");
  do {
#ifdef OS_WIN32
	  rfcomm_socket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
#else
	  rfcomm_socket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
#endif
    if (-1 == rfcomm_socket) {
      LOG4CXX_ERROR_WITH_ERRNO(
          logger_,
          "Failed to create RFCOMM socket for device " << device_handle());
      *error = new ConnectError();
#ifdef OS_WIN32
	  LOG4CXX_INFO(logger_, "exit (#" << pthread_self().x << ")");
#else
	  LOG4CXX_INFO(logger_, "exit (#" << pthread_self() << ")");
#endif
      return false;
    }
#ifdef OS_WIN32
	connect_status = connect(rfcomm_socket, (struct sockaddr *)&remoteSocketAddress, sizeof(bdaddr_t));
#else
	connect_status = ::connect(rfcomm_socket,
		(struct sockaddr*) &remoteSocketAddress,
		sizeof(remoteSocketAddress));
#endif
    if (0 == connect_status) {
      LOG4CXX_INFO(logger_, "rfcomm Connect ok");
			printf("rfcomm Connect ok\n");
      break;
    }
    LOG4CXX_INFO(logger_, "rfcomm Connect errno " << errno);
		printf("rfcomm Connect errno\n");
#ifdef OS_WIN32
	closesocket(rfcomm_socket);
#elif defined(OS_ANDROID)
  if (errno != 111 && errno != 104 && errno != 115) {
    break;
  }
  if (errno) {
    close(rfcomm_socket);
  }
#else
	if (errno != 111 && errno != 104) {
		break;
	}
	if (errno) {
		close(rfcomm_socket);
	}
#endif
#ifdef OS_WIN32
	Sleep(2 * 1000);
#else
	sleep(2);
#endif
  } while (--attempts > 0);
  LOG4CXX_INFO(logger_, "rfcomm Connect attempts finished");
	printf("rfcomm Connect attempts finished\n");
  if (0 != connect_status) {
#ifdef OS_WIN32
    LOG4CXX_ERROR_WITH_ERRNO(
        logger_,
        "Failed to Connect to remote device " << BluetoothDevice::GetUniqueDeviceId(remoteSocketAddress) << " for session " << this);
#else
		LOG4CXX_ERROR_WITH_ERRNO(
			logger_,
			"Failed to Connect to remote device " << BluetoothDevice::GetUniqueDeviceId(remoteSocketAddress.rc_bdaddr) << " for session " << this);
#endif
		printf("Failed to Connect to remote device  for session\n");
    *error = new ConnectError();
#ifdef OS_WIN32
	LOG4CXX_INFO(logger_, "exit (#" << pthread_self().x << ")");
#else
	LOG4CXX_INFO(logger_, "exit (#" << pthread_self() << ")");
#endif
    return false;
  }

  set_socket(rfcomm_socket);
#ifdef OS_WIN32
  LOG4CXX_INFO(logger_, "exit (#" << pthread_self().x << ")");
#else
  LOG4CXX_INFO(logger_, "exit (#" << pthread_self() << ")");
#endif
  return true;
}

}  // namespace transport_adapter
}  // namespace transport_manager

