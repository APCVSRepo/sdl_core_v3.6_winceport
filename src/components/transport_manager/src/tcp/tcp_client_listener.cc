/**
 * \file tcp_client_listener.cc
 * \brief TcpClientListener class source file.
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
#include "transport_manager/tcp/tcp_client_listener.h"

#ifdef OS_WIN32

#elif defined(OS_MAC)
#include <memory.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/socket.h>
#  include <sys/time.h>
#  include <netinet/in.h>
#else
#include <memory.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#ifndef  OS_ANDROID
#include <sys/sysctl.h>
#endif
#include <sys/socket.h>
#ifdef __linux__
#  include <linux/tcp.h>
#else  // __linux__
#  include <sys/time.h>
#  include <netinet/in.h>
#  include <netinet/tcp.h>
#  include <netinet/tcp_var.h>
#endif  // __linux__
#endif
#include "utils/logger.h"

#include "transport_manager/transport_adapter/transport_adapter_controller.h"
#include "transport_manager/tcp/tcp_device.h"
#include "transport_manager/tcp/tcp_socket_connection.h"

#ifdef OS_WIN32
#define SIO_KEEPALIVE_VALS _WSAIOW(IOC_VENDOR,4)  
#endif

namespace transport_manager {
namespace transport_adapter {

CREATE_LOGGERPTR_GLOBAL(logger_, "TransportManager")

#ifdef OS_WIN32
	typedef int socklen_t;
	struct TCP_KEEPALIVE {
		unsigned long onoff;
		unsigned long keepalivetime;
		unsigned long keepaliveinterval;
	};
#endif

TcpClientListener::TcpClientListener(TransportAdapterController* controller,
                                     const uint16_t port,
                                     const bool enable_keepalive)
    : port_(port),
      enable_keepalive_(enable_keepalive),
      controller_(controller),
      thread_(),
      socket_(-1),
      thread_started_(false),
      shutdown_requested_(false),
      thread_stop_requested_(false) {}

void* tcpClientListenerThread(void* data) {
  TcpClientListener* tcpClientListener = static_cast<TcpClientListener*>(data);
  assert(tcpClientListener != 0);
  tcpClientListener->Thread();
  return 0;
}

TransportAdapter::Error TcpClientListener::Init() {
  return TransportAdapter::OK;
}

void TcpClientListener::Terminate() {
  shutdown_requested_ = true;
  if (TransportAdapter::OK != StopListening()) {
    LOG4CXX_ERROR(logger_, "Cannot stop listening TCP");
  }
}

bool TcpClientListener::IsInitialised() const {
  return true;
}

TcpClientListener::~TcpClientListener() {
  LOG4CXX_INFO(logger_, "destructor");
}

void SetKeepaliveOptions(const int fd) {
  int yes = 1;
#ifdef OS_WIN32
	setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (const char *)&yes, sizeof(yes));

	TCP_KEEPALIVE inKeepAlive = { 0 };	// in parameter
	unsigned long ulInLen = sizeof(TCP_KEEPALIVE);
	TCP_KEEPALIVE outKeepAlive = { 0 }; // out parameter
	unsigned long ulOutLen = sizeof(TCP_KEEPALIVE);
	unsigned long ulBytesReturn = 0;

	inKeepAlive.onoff = 1;
	inKeepAlive.keepaliveinterval = 3000;
	inKeepAlive.keepalivetime = 1000;
	if (WSAIoctl((unsigned int)fd, SIO_KEEPALIVE_VALS,
		(LPVOID)&inKeepAlive, ulInLen,
		(LPVOID)&outKeepAlive, ulOutLen,
		&ulBytesReturn, NULL, NULL) == SOCKET_ERROR)
	{
		printf("WSAIoctl failed. error code(%d)!\n", WSAGetLastError());
		return;
	}
#else
  int keepidle = 3;  // 3 seconds to disconnection detecting
  int keepcnt = 5;
  int keepintvl = 1;
  int user_timeout = 7000;  // milliseconds
#ifdef __linux__
  setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes));
  setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(keepidle));
  setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &keepcnt, sizeof(keepcnt));
  setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &keepintvl, sizeof(keepintvl));
#ifndef OS_ANDROID
  setsockopt(fd, IPPROTO_TCP, TCP_USER_TIMEOUT, &user_timeout,
             sizeof(user_timeout));
#endif
#elif __QNX__  // __linux__
  // TODO (KKolodiy): Out of order!
  const int kMidLength = 4;
  int mib[kMidLength];
  timeval tval;

  mib[0] = CTL_NET;
  mib[1] = AF_INET;
  mib[2] = IPPROTO_TCP;
  mib[3] = TCPCTL_KEEPIDLE;
  sysctl(mib, kMidLength, NULL, NULL, &keepidle, sizeof(keepidle));

  mib[0] = CTL_NET;
  mib[1] = AF_INET;
  mib[2] = IPPROTO_TCP;
  mib[3] = TCPCTL_KEEPCNT;
  sysctl(mib, kMidLength, NULL, NULL, &keepcnt, sizeof(keepcnt));

  mib[0] = CTL_NET;
  mib[1] = AF_INET;
  mib[2] = IPPROTO_TCP;
  mib[3] = TCPCTL_KEEPINTVL;
  sysctl(mib, kMidLength, NULL, NULL, &keepintvl, sizeof(keepintvl));

  memset(&tval, sizeof(tval), 0);
  tval.tv_sec = keepidle;
  setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(yes));
  setsockopt(fd, IPPROTO_TCP, TCP_KEEPALIVE, &tval, sizeof(tval));
#endif  // __QNX__
#endif
}

void TcpClientListener::Thread() {
  LOG4CXX_INFO(logger_, "Tcp client listener thread started");

  while (false == thread_stop_requested_) {
    sockaddr_in client_address;
    socklen_t client_address_size = sizeof(client_address);
    const int connection_fd = accept(socket_, (struct sockaddr*)&client_address,
                                     &client_address_size);
    if (thread_stop_requested_) break;

    if (connection_fd < 0) {
      LOG4CXX_ERROR_WITH_ERRNO(logger_, "accept() failed");
      continue;
    }

    if (AF_INET != client_address.sin_family) {
      LOG4CXX_ERROR(logger_, "Address of connected client is invalid");
      continue;
    }

    char device_name[32];
    strncpy(device_name, inet_ntoa(client_address.sin_addr),
            sizeof(device_name) / sizeof(device_name[0]));
    LOG4CXX_INFO(logger_, "Connected client " << device_name);

    if (enable_keepalive_) SetKeepaliveOptions(connection_fd);

    TcpDevice* tcp_device = new TcpDevice(client_address.sin_addr.s_addr, device_name);
    DeviceSptr device = controller_->AddDevice(tcp_device);
    tcp_device = static_cast<TcpDevice*>(device.get());
    const ApplicationHandle app_handle = tcp_device->AddIncomingApplication(
        connection_fd);

    TcpSocketConnection* connection(
        new TcpSocketConnection(device->unique_device_id(), app_handle,
                                controller_));
    connection->set_socket(connection_fd);
    const TransportAdapter::Error error = connection->Start();
    if (error != TransportAdapter::OK) {
      delete connection;
    }
  }

  LOG4CXX_INFO(logger_, "Tcp client listener thread finished");
}

TransportAdapter::Error TcpClientListener::StartListening() {
  if (thread_started_)
    return TransportAdapter::BAD_STATE;

  socket_ = socket(AF_INET, SOCK_STREAM, 0);

  if (-1 == socket_) {
    LOG4CXX_ERROR_WITH_ERRNO(logger_, "Failed to create socket");
    return TransportAdapter::FAIL;
  }

  sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port_);
  server_address.sin_addr.s_addr = INADDR_ANY;

#ifdef OS_WIN32
  const char optval = 1;
#else
  int optval = 1;
#endif
  setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  if (0 != bind(socket_, (sockaddr*) &server_address, sizeof(server_address))) {
    LOG4CXX_ERROR_WITH_ERRNO(logger_, "bind() failed");
    return TransportAdapter::FAIL;
  }

  if (0 != listen(socket_, 128)) {
    LOG4CXX_ERROR_WITH_ERRNO(logger_, "listen() failed");
    return TransportAdapter::FAIL;
  }

  const int thread_start_error = pthread_create(&thread_, 0,
                                                &tcpClientListenerThread, this);
  if (0 == thread_start_error) {
    thread_started_ = true;
    LOG4CXX_INFO(logger_, "Tcp client listener thread started");
  } else {
    LOG4CXX_ERROR(
        logger_,
        "Tcp client listener thread start failed, error code "
            << thread_start_error);
    return TransportAdapter::FAIL;
  }
  return TransportAdapter::OK;
}

TransportAdapter::Error TcpClientListener::StopListening() {
  if (!thread_started_)
    return TransportAdapter::BAD_STATE;

  thread_stop_requested_ = true;
  int byebyesocket = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port_);
  server_address.sin_addr.s_addr = INADDR_ANY;
  connect(byebyesocket, (sockaddr*)&server_address, sizeof(server_address));
#ifdef OS_WIN32
  shutdown(byebyesocket, 2);
  closesocket(byebyesocket);
#else
  shutdown(byebyesocket, SHUT_RDWR);
  close(byebyesocket);
#endif
  pthread_join(thread_, 0);
  LOG4CXX_INFO(logger_, "Tcp client listener thread terminated");
#ifdef OS_WIN32
  closesocket(socket_);
#else
  close(socket_);
#endif
  socket_ = -1;
  thread_started_ = false;
  thread_stop_requested_ = false;
  return TransportAdapter::OK;
}

}  // namespace transport_adapter
}  // namespace transport_manager
