# Copyright (c) 2013, Ford Motor Company
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following
# disclaimer in the documentation and/or other materials provided with the
# distribution.
#
# Neither the name of the Ford Motor Company nor the names of its contributors
# may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

if(CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
include_directories (
  ${CMAKE_SOURCE_DIR}/src/thirdPartyLibs/pthread-win32-2.9.1
)
endif()

# 9--- Interfaces
add_subdirectory(./interfaces)

# 5--- TransportManager
add_subdirectory(./transport_manager)

# 3--- Policies
add_subdirectory(./resumption)

# 8--- formatters
add_subdirectory(./formatters)

# 7--- ProtocolHandler
add_subdirectory(./protocol_handler)

# 10--- ConnectionHandler
add_subdirectory(./connection_handler)

# 1--- Utils
add_subdirectory(./utils)

# 11--- Policy_impl
add_subdirectory(./policy)

# 2--- Validated types
add_subdirectory(./rpc_base)

# 4--- SmartObjects
add_subdirectory(./smart_objects)

# 12--- Application manager
add_subdirectory(./application_manager)

# 13--- Request watchdog
add_subdirectory(./request_watchdog)

# 14--- HMI Message Handler
add_subdirectory(./hmi_message_handler)

# 6--- Config
add_subdirectory(./config_profile)

# 15--- AudioManager
add_subdirectory(./media_manager)

if (TIME_TESTER)
    add_subdirectory(./time_tester)
endif()


if (${HMI_DBUS_API})
# --- DBus
    add_subdirectory(./dbus)
endif ()
if (${QT_HMI})
# --- Qt HMI
    add_subdirectory(./qt_hmi)
endif ()

