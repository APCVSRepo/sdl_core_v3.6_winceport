/**
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
#include <errno.h>
#include <unistd.h>
#include "utils/logger.h"
#include "config_profile/profile.h"
#include "media_manager/sharedmem_streamer_adapter.h"
#include "utils/Accessory_DLL.h"
#ifdef MODIFY_FUNCTION_SIGN
#ifdef ENABLE_LOG
#include "utils/file_system.h"
std::ofstream* pipe_file_stream_;
#endif
#endif

namespace media_manager {

CAccessory_DLL* g_AccessoryDll = NULL;

const static BYTE StartCommunication[5] = {0x53, 0x54, 0x41, 0x52, 0x54}; //START

void AccessoryReceive(enumCallingProcess iMsgType, const BYTE* chBuff, int iBufLen)
{

}

CREATE_LOGGERPTR_GLOBAL(logger, "SharedMemStreamerAdapter")

SharedMemStreamerAdapter::SharedMemStreamerAdapter()
  : is_ready_(false),
    messages_(),
    thread_(NULL){
  LOG4CXX_INFO(logger, "SharedMemStreamerAdapter::SharedMemStreamerAdapter");
}

SharedMemStreamerAdapter::~SharedMemStreamerAdapter() {
  LOG4CXX_INFO(logger, "SharedMemStreamerAdapter::~SharedMemStreamerAdapter");

  if ((0 != current_application_ ) && (is_ready_)) {
    StopActivity(current_application_);
  }

  thread_->stop();
  delete thread_;
}

void SharedMemStreamerAdapter::SendData(
  int32_t application_key,
  const protocol_handler::RawMessagePtr& message) {
  LOG4CXX_INFO(logger, "SharedMemStreamerAdapter::SendData");

  if (application_key != current_application_) {
    LOG4CXX_WARN(logger, "Wrong application " << application_key);
    return;
  }

  if (is_ready_) {
    messages_.push(message);
  }
}

void SharedMemStreamerAdapter::StartActivity(int32_t application_key) {
  LOG4CXX_INFO(logger, "SharedMemStreamerAdapter::StartActivity");

  if (application_key == current_application_) {
    LOG4CXX_WARN(logger, "Already started activity for " << application_key);
    return;
  }

  current_application_ = application_key;
  is_ready_ = true;

  for (std::set<MediaListenerPtr>::iterator it = media_listeners_.begin();
       media_listeners_.end() != it;
       ++it) {
    (*it)->OnActivityStarted(application_key);
  }

  LOG4CXX_TRACE(logger, "Pipe was opened for writing " << named_pipe_path_);
}

void SharedMemStreamerAdapter::StopActivity(int32_t application_key) {
  LOG4CXX_INFO(logger, "SharedMemStreamerAdapter::StopActivity");

  if (application_key != current_application_) {
    LOG4CXX_WARN(logger, "Not performing activity for " << application_key);
    return;
  }

  is_ready_ = false;
  current_application_ = 0;

  for (std::set<MediaListenerPtr>::iterator it = media_listeners_.begin();
       media_listeners_.end() != it;
       ++it) {
    (*it)->OnActivityEnded(application_key);
  }
}

bool SharedMemStreamerAdapter::is_app_performing_activity(
  int32_t application_key) {
  return (application_key == current_application_);
}

void SharedMemStreamerAdapter::Init() {
  if (!thread_) {
    LOG4CXX_INFO(logger, "Create and start sending thread");
    thread_ = new threads::Thread("SharedMemStreamerAdapter", new Streamer(this));
    const size_t kStackSize = 16384;
    thread_->startWithOptions(threads::ThreadOptions(kStackSize));
  }
}

SharedMemStreamerAdapter::Streamer::Streamer(
  SharedMemStreamerAdapter* server)
  : server_(server),
    stop_flag_(false) {
}

SharedMemStreamerAdapter::Streamer::~Streamer() {
  server_ = NULL;
}

void SharedMemStreamerAdapter::Streamer::threadMain() {
  LOG4CXX_INFO(logger, "Streamer::threadMain");

  open();

  while (!stop_flag_) {
    while (!server_->messages_.empty()) {
      protocol_handler::RawMessagePtr msg = server_->messages_.pop();
      if (!msg) {
        LOG4CXX_ERROR(logger, "Null pointer message");
        continue;
      }

#ifdef MODIFY_FUNCTION_SIGN
#ifdef ENABLE_LOG
  file_system::Write(pipe_file_stream_, msg.get()->data(), msg.get()->data_size());
#endif
#endif

		ssize_t ret = 0;
		if (g_AccessoryDll)
		{
			//PRINTMSG(1, (L"\nvideostream ready to write data(size is %d):", msg.get()->data_size()));
#ifdef BUILD_TARGET_LIB
			s_mediaVideoStreamSendCallback(msg.get()->data(), msg.get()->data_size());
			ret = msg.get()->data_size();
#else
			ret = g_AccessoryDll->AccessoryAPI_Write(eVideoStreamerAPP, eVideoStreamerChannel, msg.get()->data(), msg.get()->data_size());
#endif
			//PRINTMSG(1, (L"\nvideostream write data(size is %d):", msg.get()->data_size()));
			//for(int i = 0; i < msg.get()->data_size(); i++){
			//	PRINTMSG(1, (L"%02x ", msg.get()->data()[i]));
			//}
			//PRINTMSG(1, (L"\n"));
		}

      if (ret == 0) {
        LOG4CXX_ERROR(logger, "Failed writing data to pipe "
                      << server_->named_pipe_path_);

        std::set<MediaListenerPtr>::iterator it =
            server_->media_listeners_.begin();
        for (;server_->media_listeners_.end() != it; ++it) {
          (*it)->OnErrorReceived(server_->current_application_, -1);
        }
      } else if (ret != msg.get()->data_size()) {
        LOG4CXX_WARN(logger, "Couldn't write all the data to pipe "
                     << server_->named_pipe_path_);
      }

      static int32_t messsages_for_session = 0;
      ++messsages_for_session;

      LOG4CXX_INFO(logger, "Handling map streaming message. This is "
                   << messsages_for_session << " the message for "
                   << server_->current_application_);
      std::set<MediaListenerPtr>::iterator it =
          server_->media_listeners_.begin();
      for (; server_->media_listeners_.end() != it; ++it) {
        (*it)->OnDataReceived(server_->current_application_,
                              messsages_for_session);
      }
    }
    server_->messages_.wait();
  }
  close();
}

bool SharedMemStreamerAdapter::Streamer::exitThreadMain() {
  LOG4CXX_INFO(logger, "Streamer::exitThreadMain");
  stop_flag_ = true;
  server_->messages_.Shutdown();
  return false;
}

void SharedMemStreamerAdapter::Streamer::open() {
	if (g_AccessoryDll == NULL)
	{
		g_AccessoryDll = new CAccessory_DLL;
		AccessoryInfo accInfo;
		accInfo.eCallPro = eVideoStreamerAPP;
		accInfo.version = 1;
		accInfo.pCallbackfunc = AccessoryReceive;
		g_AccessoryDll->AccessoryAPI_Init(&accInfo);
	}
}

void SharedMemStreamerAdapter::Streamer::close() {
	if (g_AccessoryDll)
	{
		g_AccessoryDll->AccessoryAPI_DeInit(eSDLApp);
		PRINTMSG(1, (L"g_AccessoryDll->AccessoryAPI_DeInit(eSDLApp);.\n"));
		delete g_AccessoryDll;
		g_AccessoryDll = NULL;
	}	
}

}  // namespace media_manager
