# Copyright (c) 2014, Ford Motor Company
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

# --- jsoncpp
add_subdirectory(./jsoncpp)

# --- MessageBroker
add_subdirectory(./MessageBroker)

# --- encryption
add_subdirectory(./encryption)

if (BUILD_USB_SUPPORT)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "Android" OR CMAKE_SYSTEM_NAME STREQUAL "WindowsCE" OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
add_subdirectory(./libusbx-1.0.16)
endif()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
# --- pthread-win32
add_subdirectory(./pthread-win32-2.9.1)
endif()

if(BUILD_BT_SUPPORT)
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
add_subdirectory(./bluez-libs-3.36)
endif()
endif()

set(3RD_PARTY_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/3rd_party")

if(ENABLE_LOG OR HMI_DBUS_API)
if(NOT CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
# --- libexpat
  add_subdirectory(expat-2.1.0)
endif()
endif()

if(ENABLE_LOG)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME STREQUAL "QNX" OR CMAKE_SYSTEM_NAME STREQUAL "Android")
# --- libapr-1
  add_subdirectory(apr-cmake)
  if(MODIFY_FUNCTION_FLAGS)
		set(APR_LIBS_DIRECTORY ${APR_LIBS_DIRECTORY} PARENT_SCOPE)
	endif()

# --- apr-util
  add_subdirectory(apr-util-cmake)
  if(MODIFY_FUNCTION_FLAGS)
		set(APR_UTIL_LIBS_DIRECTORY ${APR_UTIL_LIBS_DIRECTORY} PARENT_SCOPE)
	endif()

# --- log4cxx
  add_subdirectory(apache-log4cxx-cmake)
  set(LOG4CXX_LIBS_DIRECTORY ${LOG4CXX_LIBS_DIRECTORY} PARENT_SCOPE)
  if(MODIFY_FUNCTION_FLAGS)
		set(LOG4CXX_INCLUDE_DIRECTORY ${LOG4CXX_INCLUDE_DIRECTORY} PARENT_SCOPE)
		set(LOG4CXX_BUILD_INCLUDE_DIRECTORY ${LOG4CXX_BUILD_INCLUDE_DIRECTORY} PARENT_SCOPE)
	endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
 if(MODIFY_FUNCTION_FLAGS)
 MESSAGE(STATUS  "current binary dir is ${CMAKE_CURRENT_BINARY_DIR}")
 MESSAGE(STATUS  "current source dir is ${CMAKE_CURRENT_SOURCE_DIR}")
     set(LOG4CXX_LIBS_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/apache-log4cxx-win32-0.10.0/apache-log4cxx-0.10.0/Release)
     set(LOG4CXX_INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/apache-log4cxx-win32-0.10.0/apache-log4cxx-0.10.0/src/main/include)
	# set(LOG4CXX_BUILD_INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/apache-log4cxx-win32-0.10.0/src/main/include)
	 
	 set(APR_LIBS_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/apache-log4cxx-win32-0.10.0/apr/Release)
	 set(APR_UTIL_LIBS_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/apache-log4cxx-win32-0.10.0/apr-util/Release)
	 
	 set(APR_LIBS_DIRECTORY ${APR_LIBS_DIRECTORY} PARENT_SCOPE)
	 set(APR_UTIL_LIBS_DIRECTORY ${APR_UTIL_LIBS_DIRECTORY} PARENT_SCOPE)
	 set(LOG4CXX_LIBS_DIRECTORY ${LOG4CXX_LIBS_DIRECTORY} PARENT_SCOPE)
	 set(LOG4CXX_INCLUDE_DIRECTORY ${LOG4CXX_INCLUDE_DIRECTORY} PARENT_SCOPE)
	 set(LOG4CXX_BUILD_INCLUDE_DIRECTORY ${LOG4CXX_BUILD_INCLUDE_DIRECTORY} PARENT_SCOPE)
 endif()
add_subdirectory(apache-log4cxx-win32-0.10.0)
endif()
endif()

if(HMI_DBUS_API)

# D-Bus
  add_subdirectory(dbus-cmake)

endif()

#libsqlite3
if (CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "WindowsCE")
  add_subdirectory(sqlite3)
endif()
