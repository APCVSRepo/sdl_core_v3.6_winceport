/**
 * \file appMain.cc
 * \brief SmartDeviceLink main application sources
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
#ifndef OS_WINCE
#include <sys/stat.h>
#endif

#ifdef OS_WIN32
#include <stdint.h>
#else
#include <unistd.h>
#endif
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>  // cpplint: Streams are highly discouraged.
#include <fstream>   // cpplint: Streams are highly discouraged.

// ----------------------------------------------------------------------------
#ifdef OS_WIN32
#include <winsock2.h>
#endif

#include "./life_cycle.h"

#include "utils/signals.h"
#include "utils/system.h"
#include "config_profile/profile.h"

#if defined(EXTENDED_MEDIA_MODE)
#include <gst/gst.h>
#endif

#include "media_manager/media_manager_impl.h"
// ----------------------------------------------------------------------------
// Third-Party includes
#include "networking.h"  // cpplint: Include the directory when naming .h files

#ifdef MODIFY_FUNCTION_SIGN
#include <lib_msp_vr.h>
#endif
#ifdef OS_WINCE
#include "utils/file_system.h"
#endif

#ifdef BUILD_TARGET_LIB
#include <main.h>
#endif
// ----------------------------------------------------------------------------

CREATE_LOGGERPTR_GLOBAL(logger, "appMain")
namespace {

const std::string kBrowser = "/usr/bin/chromium-browser";
const std::string kBrowserName = "chromium-browser";
const std::string kBrowserParams = "--auth-schemes=basic,digest,ntlm";
const std::string kLocalHostAddress = "127.0.0.1";
const std::string kApplicationVersion = "SDL_RB_B3.5";

#ifdef WEB_HMI
/**
 * Initialize HTML based HMI.
 * @return true if success otherwise false.
 */
bool InitHmi() {
#ifdef OS_WINCE
	LPWIN32_FIND_DATA  sb;
	if(INVALID_HANDLE_VALUE==FindFirstFile((LPCWSTR)"hmi_link",sb)){
		LOG4CXX_FATAL(logger, "File with HMI link doesn't exist!");
		return false;
	}
#else
struct stat sb;
if (stat("hmi_link", &sb) == -1) {
  LOG4CXX_FATAL(logger, "File with HMI link doesn't exist!");
  return false;
}
#endif

std::ifstream file_str;
file_str.open("hmi_link");

if (!file_str.is_open()) {
  LOG4CXX_FATAL(logger, "File with HMI link was not opened!");
  return false;
}

file_str.seekg(0, std::ios::end);
int32_t length = file_str.tellg();
file_str.seekg(0, std::ios::beg);

std::string hmi_link;
std::getline(file_str, hmi_link);


LOG4CXX_INFO(logger,
             "Input string:" << hmi_link << " length = " << hmi_link.size());
#ifdef OS_WIN32
#ifdef close
#undef close
file_str.close();
#define close closesocket
#endif
#else
file_str.close();
#endif

#ifdef OS_WIN32
#elif defined(OS_MAC)
#else
if (stat(hmi_link.c_str(), &sb) == -1) {
  LOG4CXX_FATAL(logger, "HMI index.html doesn't exist!");
  return false;
}
#endif
  return utils::System(kBrowser, kBrowserName).Add(kBrowserParams).Add(hmi_link)
      .Execute();
}
#endif  // WEB_HMI

#ifdef QT_HMI
/**
 * Initialize HTML based HMI.
 * @return true if success otherwise false.
 */
bool InitHmi() {
  std::string kStartHmi = "./start_hmi.sh";
  struct stat sb;
  if (stat(kStartHmi.c_str(), &sb) == -1) {
    LOG4CXX_FATAL(logger, "HMI start script doesn't exist!");
    return false;
  }

  return utils::System(kStartHmi).Execute();
}
#endif  // QT_HMI

}

/**
 * \brief Entry point of the program.
 * \param argc number of argument
 * \param argv array of arguments
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
#ifndef BUILD_TARGET_LIB
int32_t main(int32_t argc, char** argv) {
#else
int32_t sdl_start(int32_t argc,char** argv){
#endif

#ifdef BUILD_TARGET_LIB
// CSmartFactory.hpp
NsSmartDeviceLink::NsJSONHandler::strings::S_MSG_PARAMS = "msg_params";
NsSmartDeviceLink::NsJSONHandler::strings::S_PARAMS = "params";
NsSmartDeviceLink::NsJSONHandler::strings::S_FUNCTION_ID = "function_id";
NsSmartDeviceLink::NsJSONHandler::strings::S_MESSAGE_TYPE = "message_type";
NsSmartDeviceLink::NsJSONHandler::strings::S_PROTOCOL_VERSION = "protocol_version";
NsSmartDeviceLink::NsJSONHandler::strings::S_PROTOCOL_TYPE = "protocol_type";
NsSmartDeviceLink::NsJSONHandler::strings::S_CORRELATION_ID = "correlation_id";
NsSmartDeviceLink::NsJSONHandler::strings::kCode = "code";
NsSmartDeviceLink::NsJSONHandler::strings::kMessage = "message";
#endif

  // --------------------------------------------------------------------------
  // Logger initialization
#ifdef OS_WINCE
	INIT_LOGGER(file_system::CurrentWorkingDirectory() + "/" + "log4cxx.properties");
#else
  INIT_LOGGER("log4cxx.properties");
#endif

  threads::Thread::SetNameForId(threads::Thread::CurrentId(), "MainThread");

  LOG4CXX_INFO(logger, "Application started!");
  LOG4CXX_INFO(logger, "Application version " << kApplicationVersion);

   // Initialize gstreamer. Needed to activate debug from the command line.
#if defined(EXTENDED_MEDIA_MODE)
  gst_init(&argc, &argv);
#endif

  // --------------------------------------------------------------------------
  // Components initialization
  if ((argc > 1)&&(0 != argv)) {
      profile::Profile::instance()->config_file_name(argv[1]);
  } else {
#ifdef OS_WINCE
      profile::Profile::instance()->config_file_name(file_system::CurrentWorkingDirectory() + "/" + "smartDeviceLink.ini");
#else
      profile::Profile::instance()->config_file_name("smartDeviceLink.ini");
#endif
  }

#ifdef __QNX__
  if (!profile::Profile::instance()->policy_turn_off()) {
    if (!utils::System("./init_policy.sh").Execute(true)) {
      LOG4CXX_ERROR(logger, "Failed initialization of policy database");
      DEINIT_LOGGER();
      exit(EXIT_FAILURE);
    }
  }
#endif  // __QNX__
	
  main_namespace::LifeCycle::instance()->StartComponents();

  // --------------------------------------------------------------------------
  // Third-Party components initialization.

  if (!main_namespace::LifeCycle::instance()->InitMessageSystem()) {
    main_namespace::LifeCycle::instance()->StopComponents();
    DEINIT_LOGGER();
    exit(EXIT_FAILURE);
  }
  LOG4CXX_INFO(logger, "InitMessageBroker successful");

  if (profile::Profile::instance()->launch_hmi()) {
    if (profile::Profile::instance()->server_address() == kLocalHostAddress) {
      LOG4CXX_INFO(logger, "Start HMI on localhost");

#ifndef NO_HMI
#if defined(OS_LINUX) || defined(OS_QNX)
      if (!InitHmi()) {
        main_namespace::LifeCycle::instance()->StopComponents();
        DEINIT_LOGGER();
        exit(EXIT_FAILURE);
      }
#endif
      LOG4CXX_INFO(logger, "InitHmi successful");
#endif // #ifndef NO_HMI
    }
  }
  // --------------------------------------------------------------------------

  utils::SubscribeToTerminateSignal(
    &main_namespace::LifeCycle::StopComponentsOnSignal);
#ifdef  MODIFY_FUNCTION_SIGN
  msp_mem_init(MSP_MEM_SERVER);
  msp_vr_init(NULL, NULL);
  msp_vr_set_state(MSP_RECORD_INIT);

#endif

#ifdef BUILD_TARGET_LIB
	return 0;
#else
#if defined(OS_WIN32)
while (true) {
	Sleep(100500 * 1000);
}
#else
	pause();
#endif
#endif

}

#ifdef BUILD_TARGET_LIB
void sdl_stop(){
	//main_namespace::LifeCycle::instance()->StopComponents();
}

void sdl_set_videostream_callback(fun_SetMediaVideoStreamSendCallback func)
{
	SetMediaVideoStreamSendCallback(func);
}
#endif